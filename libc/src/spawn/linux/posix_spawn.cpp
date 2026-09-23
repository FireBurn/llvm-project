//===-- Linux implementation of posix_spawn -------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/spawn/posix_spawn.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/chdir.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/close.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/dup2.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/execve.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/exit_group.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/fchdir.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/fcntl.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/getgid.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/getuid.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/open.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/pipe2.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/read.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/rt_sigaction.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/rt_sigprocmask.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/sched_setparam.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/sched_setscheduler.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/setgid.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/setpgid.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/setsid.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/setuid.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/wait4.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/write.h"
#include "src/__support/OSUtil/syscall.h" // For internal syscall function.
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/spawn/file_actions.h"
#include "src/spawn/posix_spawnp.h"
#include "src/unistd/linux/exec_utils.h"

#include "hdr/fcntl_macros.h"
#include "hdr/signal_macros.h" // For SIGCHLD
#include "hdr/signal_macros.h"
#include "hdr/spawn_macros.h"
#include "hdr/types/mode_t.h"
#include "hdr/types/sigset_t.h"
#include "src/signal/linux/signal_utils.h"
#include "src/signal/sigismember.h"
#include <spawn.h>
#include <sys/syscall.h> // For syscall numbers.

namespace LIBC_NAMESPACE_DECL {

namespace {

pid_t fork() {
  // Block signal and stop abort sigaction modification.
  SigAbortGuard guard(/*exclusive=*/false);
  // TODO: Use only the clone syscall and use a sperate small stack in the child
  // to avoid duplicating the complete stack from the parent. A new stack will
  // be created on exec anyway so duplicating the full stack is unnecessary.
#ifdef SYS_fork
  return LIBC_NAMESPACE::syscall_impl<pid_t>(SYS_fork);
#elif defined(SYS_clone)
  return LIBC_NAMESPACE::syscall_impl<pid_t>(SYS_clone, SIGCHLD, 0);
#else
#error "fork or clone syscalls not available."
#endif
}

// Where the child says why it could not run. Set once, in the child, before
// anything that can fail, and read by nothing else: after the fork the two
// processes no longer share it.
int notify_fd = -1;

// Every way out of child_process is a failure. The reason goes back up the
// pipe first, so the parent can answer with it, and then the child stops.
// Anything that goes wrong writing it is not worth reporting: the exit status
// is left saying the child did not run.
[[noreturn]] void exit(int error = 0) {
  if (notify_fd >= 0 && error != 0)
    linux_syscalls::write(notify_fd, &error, sizeof(error));
  linux_syscalls::exit_group(127);
}

// Puts into effect the parts of |attr| the flags asked for. This runs in
// the child, between the fork and the exec, so anything it does is undone
// by the exec except for what the exec keeps: the ids, the process group,
// the session, the signal dispositions and mask, and the scheduling.
void apply_attributes(const posix_spawnattr_t *attr) {
  if (attr == nullptr || attr->__flags == 0)
    return;
  const short flags = attr->__flags;

  if (flags & POSIX_SPAWN_SETSID) {
    ErrorOr<pid_t> result = linux_syscalls::setsid();
    if (!result)
      exit(result.error());
  }

  if (flags & POSIX_SPAWN_SETPGROUP) {
    ErrorOr<int> result = linux_syscalls::setpgid(0, attr->__pgroup);
    if (!result)
      exit(result.error());
  }

  if (flags & POSIX_SPAWN_RESETIDS) {
    // Give up whatever the exec of this program granted, so the child runs
    // as whoever asked for it.
    gid_t gid = linux_syscalls::getgid();
    uid_t uid = linux_syscalls::getuid();
    ErrorOr<int> result = linux_syscalls::setgid(gid);
    if (result)
      result = linux_syscalls::setuid(uid);
    if (!result)
      exit(result.error());
  }

  if (flags & POSIX_SPAWN_SETSIGDEF) {
    // A handler is not carried over an exec anyway, but a signal set to be
    // ignored is, so the ones named here are put back to the default.
    struct KernelSigaction {
      void *handler;
      unsigned long sa_flags;
      void (*restorer)(void);
      sigset_t mask;
    } action{};
    action.handler = nullptr; // SIG_DFL
    for (int sig = 1; sig < NSIG; ++sig) {
      if (!LIBC_NAMESPACE::sigismember(&attr->__sigdefault, sig))
        continue;
      linux_syscalls::rt_sigaction(sig, &action, nullptr);
    }
  }

  if (flags & POSIX_SPAWN_SETSCHEDULER) {
    ErrorOr<int> result = linux_syscalls::sched_setscheduler(
        0, attr->__policy, &attr->__schedparam);
    if (!result)
      exit(result.error());
  } else if (flags & POSIX_SPAWN_SETSCHEDPARAM) {
    ErrorOr<int> result =
        linux_syscalls::sched_setparam(0, &attr->__schedparam);
    if (!result)
      exit(result.error());
  }

  // The mask goes last, so that anything above which needed a signal
  // through still had one.
  if (flags & POSIX_SPAWN_SETSIGMASK) {
    ErrorOr<int> result =
        linux_syscalls::rt_sigprocmask(SIG_SETMASK, &attr->__sigmask, nullptr);
    if (!result)
      exit(result.error());
  }
}

void child_process(const char *__restrict path,
                   const posix_spawn_file_actions_t *file_actions,
                   const posix_spawnattr_t *__restrict attr,
                   char *const *__restrict argv, char *const *__restrict envp,
                   bool search_path) {
  // TODO: In the code below, the child_process just exits on error during
  // processing |file_actions| and |attr|. The correct way would be to exit
  // after conveying the information about the failure to the parent process
  // (via a pipe for example).

  apply_attributes(attr);

  if (file_actions != nullptr) {
    auto *act = reinterpret_cast<BaseSpawnFileAction *>(file_actions->__front);
    while (act != nullptr) {
      switch (act->type) {
      case BaseSpawnFileAction::OPEN: {
        auto *open_act = reinterpret_cast<SpawnFileOpenAction *>(act);
        ErrorOr<int> fd = linux_syscalls::open(open_act->path, open_act->oflag,
                                               open_act->mode);
        if (!fd)
          exit(fd.error());
        int actual_fd = *fd;
        if (actual_fd != open_act->fd) {
          auto dup2_result = linux_syscalls::dup2(actual_fd, open_act->fd);
          linux_syscalls::close(actual_fd); // The old fd is not needed anymore.
          if (!dup2_result)
            exit(dup2_result.error());
        }
        break;
      }
      case BaseSpawnFileAction::CLOSE: {
        auto *close_act = reinterpret_cast<SpawnFileCloseAction *>(act);
        linux_syscalls::close(close_act->fd);
        break;
      }
      case BaseSpawnFileAction::DUP2: {
        auto *dup2_act = reinterpret_cast<SpawnFileDup2Action *>(act);
        // Where the two are the same descriptor there is nothing to
        // duplicate, and dup2 would do nothing at all. What POSIX asks for
        // here is that the descriptor be left open across the exec, so the
        // close-on-exec flag is what actually has to be cleared. A caller
        // handing a descriptor to a child it is about to start says so this
        // way.
        if (dup2_act->fd == dup2_act->newfd) {
          ErrorOr<int> flags = linux_syscalls::fcntl(dup2_act->fd, F_GETFD);
          if (!flags)
            exit(flags.error());
          ErrorOr<int> set = linux_syscalls::fcntl(
              dup2_act->fd, F_SETFD,
              reinterpret_cast<void *>(*flags & ~FD_CLOEXEC));
          if (!set)
            exit(set.error());
          break;
        }
        auto result = linux_syscalls::dup2(dup2_act->fd, dup2_act->newfd);
        if (!result)
          exit(result.error());
        break;
      }
      case BaseSpawnFileAction::CHDIR: {
        auto *chdir_act = reinterpret_cast<SpawnFileChdirAction *>(act);
        ErrorOr<int> result = linux_syscalls::chdir(chdir_act->path);
        if (!result)
          exit(result.error());
        break;
      }
      case BaseSpawnFileAction::FCHDIR: {
        auto *fchdir_act = reinterpret_cast<SpawnFileFchdirAction *>(act);
        ErrorOr<int> result = linux_syscalls::fchdir(fchdir_act->fd);
        if (!result)
          exit(result.error());
        break;
      }
      }
      act = act->next;
    }
  }

  // posix_spawnp looks along PATH for a name with no directory in it, which
  // is the only thing that sets it apart from posix_spawn.
  if (search_path) {
    auto result = internal::execvpe(path, argv, envp);
    exit(result ? 0 : result.error());
  }

  ErrorOr<int> result = linux_syscalls::execve(path, argv, envp);
  exit(result.error());
}

} // anonymous namespace

namespace {

int spawn(pid_t *__restrict pid, const char *__restrict path,
          const posix_spawn_file_actions_t *file_actions,
          const posix_spawnattr_t *__restrict attr,
          char *const *__restrict argv, char *const *__restrict envp,
          bool search_path) {
  // A pipe the child says why it could not run down. It is closed on exec, so
  // a child that got as far as running leaves the read end at end of file and
  // says nothing. Without this the caller is told a child started and only
  // finds out otherwise from its exit status, which is too late to tell a
  // program that is not there from one that ran and failed.
  int notify[2];
  ErrorOr<int> piped = linux_syscalls::pipe2(notify, O_CLOEXEC);
  if (!piped)
    return piped.error();

  pid_t cpid = fork();
  if (cpid == 0) {
    linux_syscalls::close(notify[0]);
    notify_fd = notify[1];
    child_process(path, file_actions, attr, argv, envp, search_path);
  }

  linux_syscalls::close(notify[1]);
  if (cpid < 0) {
    linux_syscalls::close(notify[0]);
    return -cpid;
  }

  int error = 0;
  for (;;) {
    ErrorOr<ssize_t> got =
        linux_syscalls::read(notify[0], &error, sizeof(error));
    if (!got && got.error() == EINTR)
      continue;
    // Anything other than the whole of an error number means the child said
    // nothing, which is what a child that ran does.
    if (!got || *got != static_cast<ssize_t>(sizeof(error)))
      error = 0;
    break;
  }
  linux_syscalls::close(notify[0]);

  if (error != 0) {
    // The child is already on its way out. It is reaped here so that a
    // failure leaves nothing behind for the caller to wait on.
    int status;
    for (;;) {
      ErrorOr<pid_t> waited = linux_syscalls::wait4(cpid, &status, 0, nullptr);
      if (waited || waited.error() != EINTR)
        break;
    }
    return error;
  }

  if (pid != nullptr)
    *pid = cpid;
  return 0;
}

} // anonymous namespace

LLVM_LIBC_FUNCTION(int, posix_spawn,
                   (pid_t *__restrict pid, const char *__restrict path,
                    const posix_spawn_file_actions_t *file_actions,
                    const posix_spawnattr_t *__restrict attr,
                    char *const *__restrict argv,
                    char *const *__restrict envp)) {
  return spawn(pid, path, file_actions, attr, argv, envp,
               /*search_path=*/false);
}

LLVM_LIBC_FUNCTION(int, posix_spawnp,
                   (pid_t *__restrict pid, const char *__restrict file,
                    const posix_spawn_file_actions_t *file_actions,
                    const posix_spawnattr_t *__restrict attr,
                    char *const *__restrict argv,
                    char *const *__restrict envp)) {
  return spawn(pid, file, file_actions, attr, argv, envp,
               /*search_path=*/true);
}

} // namespace LIBC_NAMESPACE_DECL
