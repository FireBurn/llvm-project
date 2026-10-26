//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Applies a credential change to every thread of the process.
///
/// The other threads are found in /proc/self/task and sent SETXID_SIGNAL.
/// Each one's handler adds the thread to a list and waits. Once a scan of
/// /proc/self/task finds no thread missing from the list, every thread makes
/// the system call, and they all return together. Holding
/// thread_creation_lock for the whole of it keeps the set of threads from
/// growing, and a thread waiting in the handler cannot exit, so the last scan
/// is complete.
///
//===----------------------------------------------------------------------===//

#include "src/__support/threads/linux/setxid.h"

#include "hdr/fcntl_macros.h"
#include "hdr/signal_macros.h"
#include "hdr/types/pid_t.h"
#include "hdr/types/sigset_t.h"
#include "hdr/types/struct_sigaction.h"
#include "hdr/unistd_macros.h"
#include "src/__support/CPP/atomic.h"
#include "src/__support/OSUtil/linux/sigmask.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/close.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/getdents64.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/getpid.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/gettid.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/lseek.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/openat.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/tgkill.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/__support/threads/callonce.h"
#include "src/__support/threads/linux/futex_utils.h"
#include "src/__support/time/abs_timeout.h"
#include "src/__support/time/clock_gettime.h"
#include "src/signal/linux/signal_utils.h"
#include "src/stdlib/abort_utils.h"
#include "src/sys/single_threaded/__libc_single_threaded.h"

#include <linux/errno.h>

namespace LIBC_NAMESPACE_DECL {
namespace internal {

namespace {

struct Waiter {
  pid_t tid;
  Waiter *next;
};

// What the handlers read. It is only written with thread_creation_lock held,
// and while no handler of an earlier broadcast is still running.
struct Broadcast {
  SetxidFn fn = nullptr;
  long args[3] = {};
  cpp::Atomic<Waiter *> waiters{nullptr};
  // How many threads are in the list, have made the call, and have left.
  Futex registered{0};
  Futex finished{0};
  Futex left{0};
  // Moves from COLLECT to RUN to RELEASE.
  Futex phase{0};
  cpp::Atomic<int> failed{0};
};

enum : FutexWordType { IDLE, COLLECT, RUN, RELEASE };

Broadcast request;
CallOnceFlag install_once = callonce_impl::NOT_CALLED;

LIBC_INLINE void wait_while(Futex &word, FutexWordType value) {
  while (word.load() == value)
    word.wait(value);
}

void handler(int, siginfo_t *, void *) {
  // A signal left queued from an earlier broadcast arrives with nothing to
  // do, as does one sent before the thread had blocked it for the handler.
  if (request.phase.load() != COLLECT)
    return;
  pid_t self = linux_syscalls::gettid();
  for (Waiter *w = request.waiters.load(); w; w = w->next)
    if (w->tid == self)
      return;

  Waiter me{self, request.waiters.load()};
  while (!request.waiters.compare_exchange_strong(me.next, &me))
    ;
  request.registered.fetch_add(1);
  request.registered.notify_one();

  wait_while(request.phase, COLLECT);
  if (!request.fn(request.args[0], request.args[1], request.args[2]))
    request.failed.store(1);
  request.finished.fetch_add(1);
  request.finished.notify_one();

  wait_while(request.phase, RUN);
  request.left.fetch_add(1);
  request.left.notify_one();
}

void install_handler() {
  struct sigaction action{};
  action.sa_sigaction = handler;
  action.sa_flags = SA_SIGINFO | SA_RESTART;
  action.sa_mask = signals::full_set();
  (void)unchecked_sigaction(SETXID_SIGNAL, &action, nullptr);
}

LIBC_INLINE bool listed(pid_t tid) {
  for (Waiter *w = request.waiters.load(); w; w = w->next)
    if (w->tid == tid)
      return true;
  return false;
}

// Signals every thread in /proc/self/task, open as |fd|, that is not in the
// list yet, and says how many it signalled.
ErrorOr<int> signal_unlisted(int fd, pid_t pid, pid_t self) {
  ErrorOr<off_t> rewound = linux_syscalls::lseek(fd, 0, SEEK_SET);
  if (!rewound)
    return Error(rewound.error());
  int signalled = 0;
  alignas(8) char buffer[2048];
  for (;;) {
    ErrorOr<ssize_t> entries =
        linux_syscalls::getdents64(fd, buffer, sizeof(buffer));
    if (!entries)
      return Error(entries.error());
    long size = *entries;
    if (size == 0)
      break;
    for (long offset = 0; offset < size;) {
      // struct linux_dirent64: d_ino, d_off, d_reclen, d_type, d_name.
      const char *entry = buffer + offset;
      unsigned short reclen;
      __builtin_memcpy(&reclen, entry + 16, sizeof(reclen));
      offset += reclen;
      const char *name = entry + 19;
      if (*name < '0' || *name > '9')
        continue;
      pid_t tid = 0;
      for (; *name >= '0' && *name <= '9'; ++name)
        tid = tid * 10 + (*name - '0');
      if (tid == self || listed(tid))
        continue;
      if (linux_syscalls::tgkill(pid, tid, SETXID_SIGNAL))
        ++signalled;
    }
  }
  return signalled;
}

// Waits for the count to pass |value|, or for a moment to go by: a thread
// signalled just as it was exiting never answers.
void wait_briefly(Futex &word, FutexWordType value) {
  timespec now;
  if (internal::clock_gettime(CLOCK_MONOTONIC, &now).has_value()) {
    now.tv_nsec += 10'000'000;
    if (now.tv_nsec >= 1'000'000'000) {
      now.tv_nsec -= 1'000'000'000;
      ++now.tv_sec;
    }
    auto timeout = AbsTimeout::from_timespec(now, /*realtime=*/false);
    if (timeout.has_value()) {
      (void)word.wait(value, timeout.value());
      return;
    }
  }
  (void)word.wait(value);
}

} // namespace

ErrorOr<int> setxid(SetxidFn fn, long arg0, long arg1, long arg2) {
  thread_creation_lock.lock();
  sigset_t old_mask;
  (void)signals::block_all(&old_mask);

  // The list of threads is opened first, so that a process without /proc is
  // told so while its credentials are still those of every thread.
  int fd = -1;
  if (!__libc_single_threaded) {
    ErrorOr<int> opened = linux_syscalls::openat(
        AT_FDCWD, "/proc/self/task", O_RDONLY | O_DIRECTORY | O_CLOEXEC, 0);
    if (!opened) {
      (void)signals::set_mask(old_mask);
      thread_creation_lock.unlock();
      return Error(opened.error());
    }
    fd = *opened;
  }

  ErrorOr<int> result = fn(arg0, arg1, arg2);
  if (!result || fd < 0) {
    if (fd >= 0)
      linux_syscalls::close(fd);
    (void)signals::set_mask(old_mask);
    thread_creation_lock.unlock();
    return result;
  }

  callonce(&install_once, install_handler);
  request.fn = fn;
  request.args[0] = arg0;
  request.args[1] = arg1;
  request.args[2] = arg2;
  request.waiters.store(nullptr);
  request.registered = 0;
  request.finished = 0;
  request.left = 0;
  request.failed.store(0);
  request.phase = COLLECT;

  pid_t pid = linux_syscalls::getpid();
  pid_t self = linux_syscalls::gettid();
  bool complete = true;
  for (;;) {
    FutexWordType before = request.registered.load();
    auto signalled = signal_unlisted(fd, pid, self);
    if (!signalled.has_value()) {
      complete = false;
      break;
    }
    if (signalled.value() == 0)
      break;
    wait_briefly(request.registered, before);
  }

  request.phase = RUN;
  request.phase.notify_all();
  FutexWordType registered = request.registered.load();
  for (FutexWordType n; (n = request.finished.load()) != registered;)
    request.finished.wait(n);

  request.phase = RELEASE;
  request.phase.notify_all();
  for (FutexWordType n; (n = request.left.load()) != registered;)
    request.left.wait(n);
  request.phase = IDLE;

  linux_syscalls::close(fd);
  (void)signals::set_mask(old_mask);
  thread_creation_lock.unlock();

  // A list that could not be read to the end leaves no knowing which
  // threads were reached.
  if (request.failed.load() || !complete)
    abort_utils::abort();
  return result;
}

} // namespace internal
} // namespace LIBC_NAMESPACE_DECL
