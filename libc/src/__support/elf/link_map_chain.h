//===-- The list of modules a debugger reads --------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC___SUPPORT_ELF_LINK_MAP_CHAIN_H
#define LLVM_LIBC_SRC___SUPPORT_ELF_LINK_MAP_CHAIN_H

#include "hdr/elf_macros.h"
#include "hdr/link_macros.h"
#include "hdr/stdint_proxy.h"
#include "hdr/types/size_t.h"
#include "hdr/types/struct_link_map.h"
#include "hdr/types/struct_r_debug.h"
#include "src/__support/elf/module.h"
#include "src/__support/macros/attributes.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {
namespace elf {

// A chain is written in one block, behind this header. The loader maps the
// first one and libc allocates the rest, so the mark says whether the block
// is libc's to free.
struct LinkMapBlock {
  uintptr_t mark;
  uintptr_t count;
};

constexpr uintptr_t LINK_MAP_HEAP_MARK = 0x6b6e696c5f646c72; // "rld_link"

LIBC_INLINE size_t link_map_block_size(size_t module_count) {
  // One more for the loader.
  return sizeof(LinkMapBlock) + (module_count + 1) * sizeof(struct link_map);
}

LIBC_INLINE struct link_map *link_maps_of(LinkMapBlock *block) {
  return reinterpret_cast<struct link_map *>(block + 1);
}

LIBC_INLINE LinkMapBlock *link_map_block_of(struct link_map *first) {
  return first == nullptr ? nullptr
                          : reinterpret_cast<LinkMapBlock *>(first) - 1;
}

// Describes the loader from its own headers. It is not one of the modules,
// but a debugger wants its symbols too. Its name is the path the executable
// asked for it by.
LIBC_INLINE bool describe_loader(const Module &executable, ElfW(Addr) base,
                                 struct link_map &map) {
  const auto *header = reinterpret_cast<const ElfW(Ehdr) *>(base);
  const auto *phdrs =
      reinterpret_cast<const ElfW(Phdr) *>(base + header->e_phoff);
  map.l_ld = nullptr;
  for (ElfW(Half) i = 0; i < header->e_phnum; ++i)
    if (phdrs[i].p_type == PT_DYNAMIC)
      map.l_ld = reinterpret_cast<ElfW(Dyn) *>(base + phdrs[i].p_vaddr);
  map.l_addr = base;
  map.l_name = const_cast<char *>("");
  for (ElfW(Half) i = 0; i < executable.phnum(); ++i)
    if (executable.phdrs()[i].p_type == PT_INTERP)
      map.l_name = reinterpret_cast<char *>(executable.load_bias() +
                                            executable.phdrs()[i].p_vaddr);
  return map.l_ld != nullptr;
}

// Writes a link_map for each module, the executable first, then one for the
// loader at `loader_base` unless that is zero, into a block with room for
// link_map_block_size(count) bytes. Returns the first.
LIBC_INLINE struct link_map *fill_link_maps(const Module *modules, size_t count,
                                            ElfW(Addr) loader_base,
                                            LinkMapBlock *block,
                                            uintptr_t mark) {
  block->mark = mark;
  struct link_map *maps = link_maps_of(block);
  size_t n = 0;
  for (; n < count; ++n) {
    maps[n].l_addr = modules[n].load_bias();
    // The executable goes by an empty name, as it does under glibc.
    maps[n].l_name = const_cast<char *>(n == 0 ? "" : modules[n].name());
    maps[n].l_ld = const_cast<ElfW(Dyn) *>(modules[n].dynamic().entries());
  }
  if (count != 0 && loader_base != 0 &&
      describe_loader(modules[0], loader_base, maps[n]))
    ++n;
  block->count = n;
  for (size_t i = 0; i < n; ++i) {
    maps[i].l_prev = i == 0 ? nullptr : &maps[i - 1];
    maps[i].l_next = i + 1 == n ? nullptr : &maps[i + 1];
  }
  return maps;
}

// A debugger finds the chain through the executable's DT_DEBUG entry.
LIBC_INLINE void point_dt_debug_at(const Module &executable,
                                   struct r_debug *debug) {
  auto *entry = const_cast<ElfW(Dyn) *>(executable.dynamic().entries());
  if (entry == nullptr)
    return;
  for (; entry->d_tag != DT_NULL; ++entry) {
    if (entry->d_tag == DT_DEBUG) {
      entry->d_un.d_ptr = reinterpret_cast<ElfW(Addr)>(debug);
      return;
    }
  }
}

} // namespace elf
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC___SUPPORT_ELF_LINK_MAP_CHAIN_H
