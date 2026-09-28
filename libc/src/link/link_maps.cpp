//===-- The list of modules a debugger reads ------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/link/link_maps.h"

#include "hdr/func/free.h"
#include "hdr/func/malloc.h"
#include "hdr/link_macros.h"
#include "src/__support/elf/link_map_chain.h"
#include "src/link/_r_debug.h"

namespace LIBC_NAMESPACE_DECL {
namespace link {

bool build_link_maps(const elf::ModuleSet &set, struct r_debug &debug,
                     ElfW(Addr) loader_base) {
  auto *block = static_cast<elf::LinkMapBlock *>(
      malloc(elf::link_map_block_size(set.count)));
  if (block == nullptr)
    return false;
  elf::LinkMapBlock *old = elf::link_map_block_of(debug.r_map);
  debug.r_map = elf::fill_link_maps(set.modules, set.count, loader_base, block,
                                    elf::LINK_MAP_HEAP_MARK);
  if (old != nullptr && old->mark == elf::LINK_MAP_HEAP_MARK)
    free(old);
  return true;
}

void publish_link_maps() {
  elf::ModuleSet *set = elf::process_modules();
  // The loader publishes the first chain. Without one there is no debugger
  // interface to keep up to date.
  if (set == nullptr || !set->linked || _r_debug.r_map == nullptr)
    return;
  build_link_maps(*set, _r_debug, _r_debug.r_ldbase);
}

void announce_link_maps(int state) {
  if (_r_debug.r_map == nullptr)
    return;
  _r_debug.r_state = state;
  reinterpret_cast<void (*)()>(_r_debug.r_brk)();
}

} // namespace link
} // namespace LIBC_NAMESPACE_DECL
