//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of makecontext for aarch64
///
//===----------------------------------------------------------------------===//

#include "src/ucontext/makecontext.h"

#include "hdr/stdint_proxy.h"
#include "include/llvm-libc-types/ucontext_t.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/stdlib/exit.h"
#include "src/ucontext/setcontext.h"

#include <stdarg.h>

namespace LIBC_NAMESPACE_DECL {

// Where a context which was made here goes when its function returns. The
// link is handed over in s1, which the called function has to preserve
// because the ABI says so.
extern "C" [[noreturn, gnu::used]] void
__llvm_libc_resume_context(ucontext_t *link) {
  if (link != nullptr)
    LIBC_NAMESPACE::setcontext(link);
  // Either there was nowhere to go, or the switch itself failed. Neither
  // leaves anything to return to, so the thread ends here.
  LIBC_NAMESPACE::exit(0);
  __builtin_unreachable();
}

namespace {

// The function returns here through ra.
__attribute__((naked)) void start_context() {
  asm("mv a0, s1\n\t"
      "call __llvm_libc_resume_context");
}

} // anonymous namespace

// Sets |ucp| up so that going to it runs |func| on the stack the caller put
// in uc_stack, with the arguments which follow. getcontext must have been
// called on |ucp| first, which is what fills in everything this does not
// touch.
LLVM_LIBC_FUNCTION(void, makecontext,
                   (ucontext_t * ucp, void (*func)(void), int argc, ...)) {
  if (ucp == nullptr || ucp->uc_stack.ss_sp == nullptr)
    return;

  // The eight registers the ABI passes the first arguments in, and then the
  // stack.
  constexpr int REGISTER_ARGS = 8;
  const int stack_args = argc > REGISTER_ARGS ? argc - REGISTER_ARGS : 0;

  uintptr_t sp =
      reinterpret_cast<uintptr_t>(ucp->uc_stack.ss_sp) + ucp->uc_stack.ss_size;

  // The arguments which go on the stack are at its bottom when the function
  // starts, and the stack pointer is sixteen byte aligned.
  sp &= ~static_cast<uintptr_t>(15);
  sp -= static_cast<uintptr_t>(stack_args) * sizeof(uintptr_t);
  sp &= ~static_cast<uintptr_t>(15);

  auto *slot = reinterpret_cast<uintptr_t *>(sp);

  va_list args;
  va_start(args, argc);
  for (int i = 0; i < argc; ++i) {
    uintptr_t value = static_cast<uintptr_t>(va_arg(args, unsigned long));
    if (i < REGISTER_ARGS)
      ucp->uc_mcontext.__gregs[10 + i] = value;
    else
      slot[i - REGISTER_ARGS] = value;
  }
  va_end(args);

  ucp->uc_mcontext.__gregs[2] = sp;
  ucp->uc_mcontext.__gregs[0] = reinterpret_cast<uintptr_t>(func);
  ucp->uc_mcontext.__gregs[1] = reinterpret_cast<uintptr_t>(&start_context);
  // What start_context reads to find where to go next.
  ucp->uc_mcontext.__gregs[9] = reinterpret_cast<uintptr_t>(ucp->uc_link);
}

} // namespace LIBC_NAMESPACE_DECL
