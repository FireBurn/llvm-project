//===-- Implementation of pthread_cancel ----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/pthread/pthread_cancel.h"

#include "hdr/errno_macros.h"
#include "hdr/signal_macros.h"
#include "hdr/types/struct_sigaction.h"
#include "src/__support/CPP/atomic.h"
#include "src/__support/OSUtil/syscall.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/__support/threads/cancel.h"
#include "src/__support/threads/thread.h"
#include "src/signal/linux/signal_utils.h"
#include "src/unistd/getpid.h"

#include <sys/syscall.h>

namespace LIBC_NAMESPACE_DECL {

namespace {

// The handler exists so the signal does not end the process, and so that a
// thread asked to stop at once does. A thread cancelled the ordinary way
// needs nothing here: the signal has already done its job by making whatever
// the thread was blocked in return, which takes it to its next cancellation
// point.
void on_cancel_signal(int) {
  ThreadAttributes *attrib = internal::self.attrib;
  if (attrib == nullptr)
    return;
  if (attrib->cancel_state.load() != uint32_t(CancelState::ENABLE))
    return;
  if (attrib->cancel_type != uint32_t(CancelType::ASYNCHRONOUS))
    return;
  internal::cancel_self();
}

cpp::Atomic<uint32_t> handler_installed;

// The handler is shared by every thread, so it is installed once. Doing it
// here rather than at startup keeps the signal free for a process that never
// cancels anything.
void install_handler() {
  uint32_t expected = 0;
  if (!handler_installed.compare_exchange_strong(expected, 1))
    return;
  struct sigaction action{};
  action.sa_handler = on_cancel_signal;
  action.sa_flags = 0; // No SA_RESTART: an interrupted call has to return.
  (void)unchecked_sigaction(internal::CANCEL_SIGNAL, &action, nullptr);
}

} // anonymous namespace

LLVM_LIBC_FUNCTION(int, pthread_cancel, (pthread_t th)) {
  auto *thread = reinterpret_cast<Thread *>(&th);
  ThreadAttributes *attrib = thread->attrib;
  if (attrib == nullptr)
    return ESRCH;

  install_handler();
  attrib->cancel_requested.store(1);

  // The request is set before the signal is sent, so a thread that reaches a
  // cancellation point without ever seeing the signal still stops. The signal
  // is only there to bring one back out of a call it would otherwise sit in.
  const int tid = attrib->tid;
  if (tid <= 0)
    return ESRCH;
  const int result = LIBC_NAMESPACE::syscall_impl<int>(
      SYS_tgkill, LIBC_NAMESPACE::getpid(), tid, internal::CANCEL_SIGNAL);
  if (result < 0)
    return -result;
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL
