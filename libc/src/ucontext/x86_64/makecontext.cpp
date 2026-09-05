//===-- x86_64 implementation of makecontext ------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
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
// link is handed over in rbx, which the called function has to preserve
// because the ABI says so.
extern "C" [[noreturn]] void __llvm_libc_resume_context(ucontext_t *link) {
  if (link != nullptr)
    LIBC_NAMESPACE::setcontext(link);
  // Either there was nowhere to go, or the switch itself failed. Neither
  // leaves anything to return to, so the thread ends here.
  LIBC_NAMESPACE::exit(0);
  __builtin_unreachable();
}

namespace {

__attribute__((naked)) void start_context() {
  asm("movq %rbx, %rdi\n\t"
      "callq __llvm_libc_resume_context");
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

  // The six registers the ABI passes the first arguments in, and then the
  // stack.
  constexpr int REGISTER_ARGS = 6;
  const int stack_args = argc > REGISTER_ARGS ? argc - REGISTER_ARGS : 0;

  uintptr_t sp =
      reinterpret_cast<uintptr_t>(ucp->uc_stack.ss_sp) + ucp->uc_stack.ss_size;

  // The block of arguments which go on the stack sits at the top of it,
  // sixteen byte aligned, and the return address goes just below. That
  // leaves the stack pointer eight past an alignment boundary, which is
  // where it stands at the entry to a function the ABI's way.
  sp &= ~static_cast<uintptr_t>(15);
  sp -= static_cast<uintptr_t>(stack_args) * sizeof(uintptr_t);
  sp &= ~static_cast<uintptr_t>(15);
  sp -= sizeof(uintptr_t);

  auto *slot = reinterpret_cast<uintptr_t *>(sp);
  slot[0] = reinterpret_cast<uintptr_t>(&start_context);

  va_list args;
  va_start(args, argc);
  greg_t *regs = ucp->uc_mcontext.gregs;
  for (int i = 0; i < argc; ++i) {
    uintptr_t value = static_cast<uintptr_t>(va_arg(args, unsigned long));
    switch (i) {
    case 0:
      regs[REG_RDI] = static_cast<greg_t>(value);
      break;
    case 1:
      regs[REG_RSI] = static_cast<greg_t>(value);
      break;
    case 2:
      regs[REG_RDX] = static_cast<greg_t>(value);
      break;
    case 3:
      regs[REG_RCX] = static_cast<greg_t>(value);
      break;
    case 4:
      regs[REG_R8] = static_cast<greg_t>(value);
      break;
    case 5:
      regs[REG_R9] = static_cast<greg_t>(value);
      break;
    default:
      slot[1 + (i - REGISTER_ARGS)] = value;
      break;
    }
  }
  va_end(args);

  regs[REG_RSP] = static_cast<greg_t>(sp);
  regs[REG_RIP] = static_cast<greg_t>(reinterpret_cast<uintptr_t>(func));
  // What start_context reads to find where to go next.
  regs[REG_RBX] =
      static_cast<greg_t>(reinterpret_cast<uintptr_t>(ucp->uc_link));
}

} // namespace LIBC_NAMESPACE_DECL
