//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of dlsym
///
//===----------------------------------------------------------------------===//

#include "src/dlfcn/dl_lookup.h"

#include "hdr/stdint_proxy.h"
#include "src/__support/CPP/mutex.h"
#include "src/__support/elf/bind.h"
#include "src/__support/elf/passive_abi.h"
#include "src/__support/macros/config.h"
#include "src/dlfcn/dl_internal.h"

namespace LIBC_NAMESPACE_DECL {
namespace dl {

namespace {

// <dlfcn.h> states the two of these as values rather than in a header this
// can include, so they are named here rather than spelled out at the tests
// below.
constexpr void *DEFAULT_HANDLE = nullptr;
constexpr uintptr_t NEXT_HANDLE = static_cast<uintptr_t>(-1l);

// Where `symbol` of `module` is, which for an indirect function is what its
// resolver picks.
void *address_of(const elf::Module &module, const ElfW(Sym) * symbol) {
  const ElfW(Addr) address = module.load_bias() + symbol->st_value;
  return reinterpret_cast<void *>(elf::symbol_is_ifunc(symbol->st_info)
                                      ? elf::SearchOrder::call_resolver(address)
                                      : address);
}

// The first module at or after `from` which defines `name`. Returns nothing
// when none does.
void *search_from(const elf::ModuleSet &set, size_t from, const char *name,
                  const char *version) {
  for (size_t i = from; i < set.count; ++i) {
    const ElfW(Sym) *symbol = set.modules[i].symbols().lookup(name, version);
    if (symbol == nullptr || symbol->st_shndx == SHN_UNDEF)
      continue;
    return address_of(set.modules[i], symbol);
  }
  return nullptr;
}

// Whether `have` is the name `want`.
bool same_name(const char *have, const char *want) {
  if (have == nullptr)
    return false;
  for (; *have == *want; ++have, ++want)
    if (*have == '\0')
      return true;
  return false;
}

// The definition of `name` in the module at `index` or in anything it needs,
// searched breadth first the way a lookup through a handle goes. A library
// that only names others, as a compatibility stub does, finds its symbols in
// what it pulls in.
void *search_handle(const elf::ModuleSet &set, size_t index, const char *name,
                    const char *version) {
  constexpr size_t LIMIT = 512;
  size_t queue[LIMIT];
  bool queued[LIMIT] = {};
  size_t head = 0;
  size_t tail = 0;
  if (index >= LIMIT || index >= set.count) {
    const ElfW(Sym) *symbol = set.modules[index].symbols().lookup(name, version);
    if (symbol == nullptr || symbol->st_shndx == SHN_UNDEF)
      return nullptr;
    return address_of(set.modules[index], symbol);
  }
  queue[tail++] = index;
  queued[index] = true;
  while (head < tail) {
    const elf::Module &module = set.modules[queue[head++]];
    const ElfW(Sym) *symbol = module.symbols().lookup(name, version);
    if (symbol != nullptr && symbol->st_shndx != SHN_UNDEF)
      return address_of(module, symbol);
    module.for_each_needed([&](const char *needed) {
      for (size_t i = 0; i < set.count && i < LIMIT; ++i) {
        if (queued[i] || (!same_name(set.modules[i].soname(), needed) &&
                          !same_name(set.modules[i].name(), needed)))
          continue;
        queued[i] = true;
        queue[tail++] = i;
        return;
      }
    });
  }
  return nullptr;
}

} // anonymous namespace

void *lookup(void *handle, const char *name, ElfW(Addr) caller,
             const char *version) {
  elf::ModuleSet *modules = elf::process_modules();
  cpp::lock_guard lock(dl::dl_mutex);

  if (modules == nullptr || !modules->linked) {
    dl::set_error("dlsym is only available in a dynamically linked process");
    return nullptr;
  }
  elf::ModuleSet &set = *modules;
  if (name == nullptr) {
    dl::set_error("dlsym was given no symbol name");
    return nullptr;
  }

  // Every module, in the order they were loaded, which is the order a lookup
  // through the program's own scope goes in.
  if (handle == DEFAULT_HANDLE) {
    if (void *found = search_from(set, 0, name, version))
      return found;
    dl::set_error("undefined symbol", name);
    return nullptr;
  }

  // Everything after whatever called, which is how a wrapper reaches the
  // definition it is standing in front of.
  if (reinterpret_cast<uintptr_t>(handle) == NEXT_HANDLE) {
    size_t after = set.count;
    for (size_t i = 0; i < set.count; ++i) {
      if (set.modules[i].contains(caller)) {
        after = i + 1;
        break;
      }
    }
    if (void *found = search_from(set, after, name, version))
      return found;
    dl::set_error("undefined symbol", name);
    return nullptr;
  }

  size_t index = 0;
  if (!dl::index_for(handle, index)) {
    dl::set_error("dlsym was given a handle that dlopen did not return");
    return nullptr;
  }

  // dlopen was given a null path for this handle, which asks for the main
  // program, and a lookup through that is a lookup through the whole of the
  // program's scope rather than the executable on its own.
  if (index == 0) {
    if (void *found = search_from(set, 0, name, version))
      return found;
    dl::set_error("undefined symbol", name);
    return nullptr;
  }

  if (void *found = search_handle(set, index, name, version))
    return found;
  dl::set_error("undefined symbol", name);
  return nullptr;
}

} // namespace dl
} // namespace LIBC_NAMESPACE_DECL
