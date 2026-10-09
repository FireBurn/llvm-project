//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of dladdr
///
//===----------------------------------------------------------------------===//

#include "src/dlfcn/dl_address.h"

#include "src/__support/CPP/mutex.h"
#include "src/__support/elf/passive_abi.h"
#include "src/__support/macros/config.h"
#include "src/dlfcn/dl_internal.h"
#include "src/link/_r_debug.h"

namespace LIBC_NAMESPACE_DECL {
namespace dl {

namespace {

// The symbol an address belongs to is the one defined closest below it. A
// symbol with a size covers only that much; one with none, which is what an
// assembly routine often has, covers up to whatever is defined next.
const ElfW(Sym) * enclosing_symbol(const elf::Module &module,
                                   ElfW(Addr) address, const char **name) {
  const elf::GnuSymbolTable table = module.symbols();
  const uint32_t count = table.count();
  const ElfW(Sym) *best = nullptr;
  ElfW(Addr) best_value = 0;

  for (uint32_t i = 0; i < count; ++i) {
    const ElfW(Sym) *symbol = table.symbol(i);
    if (symbol->st_shndx == SHN_UNDEF || symbol->st_name == 0)
      continue;
    const ElfW(Addr) value = module.load_bias() + symbol->st_value;
    if (value > address)
      continue;
    if (symbol->st_size != 0 && address >= value + symbol->st_size)
      continue;
    if (best == nullptr || value > best_value) {
      best = symbol;
      best_value = value;
      *name = table.name(i);
    }
  }
  return best;
}

} // anonymous namespace

struct link_map *link_map_for(ElfW(Addr) bias) {
  for (struct link_map *map = _r_debug.r_map; map != nullptr; map = map->l_next)
    if (map->l_addr == bias)
      return map;
  return nullptr;
}

bool address_lookup(const void *addr, Dl_info *info, const ElfW(Sym) * *symbol,
                    ElfW(Addr) * bias) {
  if (info == nullptr)
    return false;

  elf::ModuleSet *modules = elf::process_modules();
  cpp::lock_guard lock(dl::dl_mutex);
  if (modules == nullptr || !modules->linked)
    return false;
  elf::ModuleSet &set = *modules;

  const ElfW(Addr) address = reinterpret_cast<ElfW(Addr)>(addr);
  const elf::Module *module = nullptr;
  for (size_t i = 0; i < set.count; ++i) {
    if (set.modules[i].contains(address)) {
      module = &set.modules[i];
      break;
    }
  }
  if (module == nullptr)
    return false;

  info->dli_fname = module->name();
  info->dli_fbase = reinterpret_cast<void *>(module->load_bias());
  info->dli_sname = nullptr;
  info->dli_saddr = nullptr;

  const char *name = nullptr;
  const ElfW(Sym) *found = enclosing_symbol(*module, address, &name);
  if (found != nullptr) {
    info->dli_sname = name;
    info->dli_saddr =
        reinterpret_cast<void *>(module->load_bias() + found->st_value);
  }
  if (symbol != nullptr)
    *symbol = found;
  if (bias != nullptr)
    *bias = module->load_bias();
  return true;
}

} // namespace dl
} // namespace LIBC_NAMESPACE_DECL
