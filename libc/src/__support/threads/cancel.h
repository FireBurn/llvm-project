//===-- Thread cancellation -------------------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC___SUPPORT_THREADS_CANCEL_H
#define LLVM_LIBC_SRC___SUPPORT_THREADS_CANCEL_H

#include "hdr/errno_macros.h"
#include "src/__support/macros/attributes.h"
#include "src/__support/macros/config.h"
#include "src/__support/threads/thread.h"

namespace LIBC_NAMESPACE_DECL {
namespace internal {

// What a cancelled thread is joined with, which is not a value any thread
// could return itself.
LIBC_INLINE void *canceled() { return reinterpret_cast<void *>(-1); }

// The signal a cancellation request is sent with. It is not for programs to
// use: its only job is to interrupt whatever the target thread is blocked in
// so that the thread reaches its next cancellation point. This is why
// SIGRTMIN is one above it.
constexpr int CANCEL_SIGNAL = 32;

// True if the calling thread has been asked to stop and is willing to.
LIBC_INLINE bool cancel_pending() {
  ThreadAttributes *attrib = self.attrib;
  return attrib != nullptr &&
         attrib->cancel_state.load() == uint32_t(CancelState::ENABLE) &&
         attrib->cancel_requested.load() != 0;
}

// Runs the calling thread's cleanup handlers and ends it. Declared here and
// defined with the rest of the thread machinery, which is what knows how to
// end one.
[[noreturn]] void cancel_self();

// A cancellation point: where a thread that has been asked to stop does so.
LIBC_INLINE void cancel_point() {
  if (cancel_pending())
    cancel_self();
}

// Makes a blocking call a cancellation point, which is what POSIX requires of
// the ones that can wait indefinitely. The request is acted on before the
// call is made and again if the call was interrupted, which is what the
// signal sent with the request causes.
template <typename F> LIBC_INLINE auto cancellable(F call) -> decltype(call()) {
  cancel_point();
  auto result = call();
  if (!result.has_value() && result.error() == EINTR)
    cancel_point();
  return result;
}

// The same, for a call that reports a negative error number the way the
// kernel does rather than through ErrorOr.
template <typename F>
LIBC_INLINE auto cancellable_raw(F call) -> decltype(call()) {
  cancel_point();
  auto result = call();
  if (result == -EINTR)
    cancel_point();
  return result;
}

} // namespace internal
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC___SUPPORT_THREADS_CANCEL_H
