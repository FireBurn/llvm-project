//===-- The list of modules a debugger reads --------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_LINK_LINK_MAPS_H
#define LLVM_LIBC_SRC_LINK_LINK_MAPS_H

#include "hdr/types/struct_link_map.h"
#include "hdr/types/struct_r_debug.h"
#include "src/__support/elf/passive_abi.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {
namespace link {

// Points `debug.r_map` at a new chain for the set, with the loader at
// `loader_base` last unless that is zero, and frees the chain it replaces if
// libc allocated it. Returns false, leaving `debug` alone, if there was no
// memory.
bool build_link_maps(const elf::ModuleSet &set, struct r_debug &debug,
                     ElfW(Addr) loader_base);

// Rebuilds _r_debug's chain after dlopen or dlclose changed the module set.
void publish_link_maps();

// Tells a debugger, through the function at _r_debug.r_brk, that the module
// list is about to change (RT_ADD or RT_DELETE) or has settled
// (RT_CONSISTENT).
void announce_link_maps(int state);

} // namespace link
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_LINK_LINK_MAPS_H
