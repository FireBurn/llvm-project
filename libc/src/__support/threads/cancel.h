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

// Acts on a request the calling thread has been given, and is defined with
// the rest of the thread machinery, which is what knows how to end one. The
// reference is weak so that a cancellation point costs a program with no
// threads in it nothing: nothing has asked it to stop, and reaching for the
// thread machinery to be told so would pull the whole of it, and the startup
// code it needs, into a link that has no use for either.
[[gnu::weak]] void reach_cancel_point();

// Runs the calling thread's cleanup handlers and ends it. Defined with the
// rest of the thread machinery. Anything calling this directly is asking for
// threads by name, so the reference is an ordinary one.
[[noreturn]] void cancel_self();

// A cancellation point: where a thread that has been asked to stop does so.
LIBC_INLINE void cancel_point() {
  if (reach_cancel_point != nullptr)
    reach_cancel_point();
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
