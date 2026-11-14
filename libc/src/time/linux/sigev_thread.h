//===-- Userspace SIGEV_THREAD timers for Linux ---------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// The kernel has no SIGEV_THREAD, so timer_create gives such a timer a thread
/// of its own and makes the kernel timer SIGEV_THREAD_ID towards it. The
/// thread waits for the signal and calls the function. The timer keeps the
/// kernel's id as its timer_t; the threads are found again by that id.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_TIME_LINUX_SIGEV_THREAD_H
#define LLVM_LIBC_SRC_TIME_LINUX_SIGEV_THREAD_H

#include "hdr/errno_macros.h"
#include "hdr/pthread_macros.h"
#include "hdr/signal_macros.h"
#include "hdr/types/clockid_t.h"
#include "hdr/types/pthread_attr_t.h"
#include "hdr/types/pthread_t.h"
#include "hdr/types/siginfo_t.h"
#include "hdr/types/sigset_t.h"
#include "hdr/types/struct_sigevent.h"
#include "hdr/types/timer_t.h"
#include "src/__support/OSUtil/linux/sigmask.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/getpid.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/rt_sigprocmask.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/rt_sigtimedwait.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/tgkill.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/timer_create.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/__support/threads/identifier.h"
#include "src/__support/threads/linux/futex_utils.h"
#include "src/__support/threads/raw_mutex.h"
#include "src/pthread/pthread_create.h"

namespace LIBC_NAMESPACE_DECL {
namespace internal {
namespace sigev_thread {

// The same signal as the one glibc and Bionic use for these timers. It is
// the library's, as CANCEL_SIGNAL is: no thread but a timer's own waits for it
// and the raw system calls below are not subject to sigprocmask's filtering.
constexpr int TIMER_SIGNAL = 32;

enum : FutexWordType { WAITING = 0, GO = 1, FAILED = 2 };

// The record of a timer lives on the stack of its thread, which is there for
// as long as the timer is: the thread ends when the timer is deleted.
struct Timer {
  Timer *next;
  int kernel_id;
  void (*function)(sigval);
  sigval value;
  pid_t tid;
  Futex start{WAITING};
  bool deleted;
};

// What timer_create gives a new thread, on the stack of timer_create, which
// waits until the thread has handed back its Timer.
struct Handoff {
  void (*function)(sigval);
  sigval value;
  Futex ready{0};
  Timer *timer;
};

LIBC_INLINE_VAR RawMutex registry_lock;
LIBC_INLINE_VAR Timer *registry = nullptr;

LIBC_INLINE void *thread_main(void *arg) {
  Handoff *handoff = static_cast<Handoff *>(arg);
  Timer self;
  self.next = nullptr;
  self.kernel_id = -1;
  self.function = handoff->function;
  self.value = handoff->value;
  self.tid = internal::gettid();
  self.start.store(WAITING);
  self.deleted = false;
  handoff->timer = &self;
  handoff->ready.store(1);
  handoff->ready.notify_one();

  // The parent is still making the kernel timer. If that fails there is
  // nothing for this thread to do.
  while (self.start.load() == WAITING)
    (void)self.start.wait(WAITING);
  if (self.start.load() == FAILED)
    return nullptr;

  sigset_t set = signals::empty_set();
  signals::add(set, TIMER_SIGNAL);
  for (;;) {
    siginfo_t info{};
    if (!linux_syscalls::rt_sigtimedwait(&set, &info, nullptr))
      continue;
    if (info.si_code == SI_TIMER) {
      // timer_delete may already have run: nothing may be called after it.
      if (!__atomic_load_n(&self.deleted, __ATOMIC_ACQUIRE))
        self.function(self.value);
    } else if (info.si_code == SI_TKILL) {
      return nullptr;
    }
  }
}

// Makes a timer of clockid that runs sev's function on a new thread, with the
// thread's attributes if sev has any. Returns 0 or an error number.
LIBC_INLINE int create(clockid_t clockid, const sigevent *sev, timer_t *out) {
  if (sev->sigev_notify_function == nullptr)
    return EINVAL;

  Handoff handoff;
  handoff.function = sev->sigev_notify_function;
  handoff.value = sev->sigev_value;
  handoff.timer = nullptr;

  pthread_attr_t attr{};
  if (sev->sigev_notify_attributes != nullptr)
    attr = *static_cast<const pthread_attr_t *>(sev->sigev_notify_attributes);
  attr.__detachstate = PTHREAD_CREATE_DETACHED;

  // The thread starts with the signal blocked, which it inherits, so that the
  // signal cannot take its default action before the thread waits for it.
  sigset_t block = signals::empty_set();
  signals::add(block, TIMER_SIGNAL);
  sigset_t old;
  (void)linux_syscalls::rt_sigprocmask(SIG_BLOCK, &block, &old);
  pthread_t thread;
  int rc =
      LIBC_NAMESPACE::pthread_create(&thread, &attr, thread_main, &handoff);
  (void)linux_syscalls::rt_sigprocmask(SIG_SETMASK, &old, nullptr);
  if (rc != 0)
    return rc;

  while (handoff.ready.load() == 0)
    (void)handoff.ready.wait(0);
  Timer *timer = handoff.timer;

  sigevent target{};
  target.sigev_notify = SIGEV_THREAD_ID;
  target.sigev_signo = TIMER_SIGNAL;
  target.sigev_value = sev->sigev_value;
  target.sigev_notify_thread_id = timer->tid;
  timer_t id = nullptr;
  auto made = linux_syscalls::timer_create(clockid, &target, &id);
  if (!made) {
    timer->start.store(FAILED);
    timer->start.notify_one();
    return made.error();
  }

  timer->kernel_id = static_cast<int>(reinterpret_cast<long>(id));
  registry_lock.lock();
  timer->next = registry;
  registry = timer;
  (void)registry_lock.unlock();
  timer->start.store(GO);
  timer->start.notify_one();
  *out = id;
  return 0;
}

// Stops the thread of the timer with this kernel id, if it has one. The
// thread ends itself once it is out of the function.
LIBC_INLINE void forget(timer_t id) {
  int kernel_id = static_cast<int>(reinterpret_cast<long>(id));
  Timer *found = nullptr;
  registry_lock.lock();
  for (Timer **link = &registry; *link != nullptr; link = &(*link)->next) {
    if ((*link)->kernel_id == kernel_id) {
      found = *link;
      *link = found->next;
      break;
    }
  }
  (void)registry_lock.unlock();
  if (found == nullptr)
    return;
  __atomic_store_n(&found->deleted, true, __ATOMIC_RELEASE);
  (void)linux_syscalls::tgkill(linux_syscalls::getpid(), found->tid,
                               TIMER_SIGNAL);
}

} // namespace sigev_thread
} // namespace internal
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_TIME_LINUX_SIGEV_THREAD_H
