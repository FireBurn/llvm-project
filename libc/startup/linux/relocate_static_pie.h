//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Relocation of a static PIE by itself
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_STARTUP_LINUX_RELOCATE_STATIC_PIE_H
#define LLVM_LIBC_STARTUP_LINUX_RELOCATE_STATIC_PIE_H

#include "hdr/elf_macros.h"
#include "hdr/elf_proxy.h"
#include "hdr/link_macros.h"
#include "hdr/stdint_proxy.h"
#include "src/__support/macros/attributes.h"
#include "src/__support/macros/config.h"

extern "C" {
// Not weak, since a weak symbol is reached through the GOT, which is what
// this is about to relocate.
[[gnu::visibility("hidden")]] extern const ElfW(Ehdr) __ehdr_start; // NOLINT
}

namespace LIBC_NAMESPACE_DECL {

#ifndef DT_RELR
#define DT_RELR 36
#define DT_RELRSZ 35
#endif

#if defined(__x86_64__)
inline constexpr uintptr_t RELATIVE_RELOC = R_X86_64_RELATIVE;
#elif defined(__aarch64__)
inline constexpr uintptr_t RELATIVE_RELOC = R_AARCH64_RELATIVE;
#elif defined(__riscv)
inline constexpr uintptr_t RELATIVE_RELOC = R_RISCV_RELATIVE;
#endif

// A program linked with -static-pie has nothing to relocate it, as the loader
// does for one it runs. The pointers in its data still hold the addresses it
// was linked for until this adds to them where it was put. Nothing before this
// may use a pointer from the data, so it keeps to the stack and registers.
LIBC_INLINE void relocate_static_pie() {
  // The image is linked at address zero.
  uintptr_t base = reinterpret_cast<uintptr_t>(&__ehdr_start);
  const ElfW(Phdr) *phdr =
      reinterpret_cast<const ElfW(Phdr) *>(base + __ehdr_start.e_phoff);
  const ElfW(Dyn) *dynamic = nullptr;
  for (ElfW(Half) i = 0; i < __ehdr_start.e_phnum; ++i) {
    // A program with an interpreter is relocated by it before it runs, and
    // doing it twice would corrupt what RELR adds to.
    if (phdr[i].p_type == PT_INTERP)
      return;
    if (phdr[i].p_type == PT_DYNAMIC)
      dynamic = reinterpret_cast<const ElfW(Dyn) *>(base + phdr[i].p_vaddr);
  }
  if (!dynamic)
    return;
  const ElfW(Rela) *rela = nullptr;
  uintptr_t rela_size = 0;
  const uintptr_t *relr = nullptr;
  uintptr_t relr_size = 0;
  for (const ElfW(Dyn) *dyn = dynamic; dyn->d_tag != DT_NULL; ++dyn) {
    switch (dyn->d_tag) {
    case DT_RELA:
      rela = reinterpret_cast<const ElfW(Rela) *>(base + dyn->d_un.d_ptr);
      break;
    case DT_RELASZ:
      rela_size = dyn->d_un.d_val;
      break;
    case DT_RELR:
      relr = reinterpret_cast<const uintptr_t *>(base + dyn->d_un.d_ptr);
      break;
    case DT_RELRSZ:
      relr_size = dyn->d_un.d_val;
      break;
    }
  }

  for (uintptr_t i = 0; i < rela_size / sizeof(ElfW(Rela)); ++i) {
    if (ELF64_R_TYPE(rela[i].r_info) != RELATIVE_RELOC)
      continue;
    *reinterpret_cast<uintptr_t *>(base + rela[i].r_offset) =
        base + static_cast<uintptr_t>(rela[i].r_addend);
  }

  // An entry with its low bit clear gives an address to relocate, and one with
  // it set is a bitmap of the words that follow, 63 to an entry.
  uintptr_t *where = nullptr;
  for (uintptr_t i = 0; i < relr_size / sizeof(uintptr_t); ++i) {
    uintptr_t entry = relr[i];
    if ((entry & 1) == 0) {
      where = reinterpret_cast<uintptr_t *>(base + entry);
      *where++ += base;
      continue;
    }
    for (uintptr_t bit = 0, bits = entry >> 1; bits; bits >>= 1, ++bit)
      if (bits & 1)
        where[bit] += base;
    where += 63;
  }
}

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_STARTUP_LINUX_RELOCATE_STATIC_PIE_H
