//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Entry point of the dynamic loader on aarch64
///
//===----------------------------------------------------------------------===//

#include "loader/linux/loader_main.h"

#include "hdr/link_macros.h"
#include "src/__support/elf/self_relocate.h"
#include "src/__support/macros/config.h"

// Hidden, so they are reached pc-relative and not through a GOT entry that has
// not been relocated yet.
extern "C" __attribute__((visibility("hidden"))) const ElfW(Dyn) _DYNAMIC[];
extern "C" __attribute__((visibility("hidden"))) const char __ehdr_start[];

namespace LIBC_NAMESPACE_DECL {

extern "C" [[gnu::used]] uintptr_t __libc_loader_start(void *stack_pointer) {
  const ElfW(Addr) load_bias = reinterpret_cast<ElfW(Addr)>(__ehdr_start);
  elf::self_relocate(_DYNAMIC, load_bias);
  // Everything from here on has a usable address.
  return loader_link(stack_pointer);
}

} // namespace LIBC_NAMESPACE_DECL

__asm__(".text\n"
        ".globl _start\n"
        ".type _start,%function\n"
        "_start:\n"
        "  mov x29, xzr\n"
        "  mov x30, xzr\n"
        "  mov x19, sp\n" // the stack is the only argument, and x19 keeps it
        "  mov x0, sp\n"
        "  bl __libc_loader_start\n"
        "  mov x9, x0\n" // the entry point it returned
        "  mov sp, x19\n" // the program gets the original stack
        "  mov x0, xzr\n" // and no finalizer to call at exit
        "  br x9\n"
        ".size _start,.-_start\n");
