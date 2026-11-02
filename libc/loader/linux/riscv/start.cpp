//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Entry point of the dynamic loader on riscv64
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

// The compiler copies larger structures, the loaded modules among them, with a
// call to memcpy, and the loader has no libc to take it from. It may be
// reached before relocation, so it is hidden like the rest.
extern "C" __attribute__((visibility("hidden"))) void *
memcpy(void *__restrict dest, const void *__restrict src, __SIZE_TYPE__ count) {
  auto *d = static_cast<unsigned char *>(dest);
  auto *s = static_cast<const unsigned char *>(src);
  for (__SIZE_TYPE__ i = 0; i < count; ++i)
    d[i] = s[i];
  return dest;
}

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
        ".type _start,@function\n"
        "_start:\n"
        "  mv s0, zero\n"
        "  mv ra, zero\n"
        "  mv s1, sp\n" // the stack is the only argument, and s1 keeps it
        "  mv a0, sp\n"
        "  call __libc_loader_start\n"
        "  mv t0, a0\n" // the entry point it returned
        "  mv sp, s1\n" // the program gets the original stack
        "  mv a0, zero\n" // and no finalizer to call at exit
        "  jr t0\n"
        ".size _start,.-_start\n");
