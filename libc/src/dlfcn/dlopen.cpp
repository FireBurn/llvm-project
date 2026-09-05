//===-- Implementation of dlopen ------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "dlopen.h"

#include "src/__support/CPP/mutex.h"
#include "src/__support/common.h"
#include "src/__support/elf/bind.h"
#include "src/__support/elf/load_module.h"
#include "src/__support/elf/passive_abi.h"
#include "src/__support/elf/search.h"
#include "src/__support/macros/config.h"
#include "src/dlfcn/dl_internal.h"
#include "src/unistd/environ.h"

namespace LIBC_NAMESPACE_DECL {

namespace {

bool same_name(const char *a, const char *b) {
  if (a == nullptr || b == nullptr)
    return false;
  for (; *a == *b; ++a, ++b)
    if (*a == '\0')
      return true;
  return false;
}

// Whether the set already holds something of this name, by either the path
// it was opened under or the name it says it goes by.
bool already_loaded(const elf::ModuleSet &set, const char *name) {
  for (size_t i = 0; i < set.count; ++i)
    if (same_name(set.modules[i].name(), name) ||
        same_name(set.modules[i].soname(), name))
      return true;
  return false;
}

void run_init_array(const elf::Module &module) {
  auto array = module.dynamic().address(DT_INIT_ARRAY);
  auto size = module.dynamic().value(DT_INIT_ARRAYSZ);
  if (!array || !size)
    return;
  auto **functions = reinterpret_cast<void (**)()>(*array);
  for (size_t i = 0; i < *size / sizeof(void *); ++i)
    if (functions[i] != nullptr)
      functions[i]();
}

} // anonymous namespace

LLVM_LIBC_FUNCTION(void *, dlopen, (const char *path, int)) {
  // The run path that applies is the one belonging to whatever called, which
  // is found from where the call is returning to. Taken before anything else,
  // so nothing stands between this and the caller.
  const ElfW(Addr) caller =
      reinterpret_cast<ElfW(Addr)>(__builtin_return_address(0));

  elf::ModuleSet &set = elf::loaded_modules();
  cpp::lock_guard lock(dl::dl_mutex);

  // Without a loader there is no module set to add to, and nothing the caller
  // opened could be linked against the program.
  if (!set.linked) {
    dl::set_error("dlopen is only available in a dynamically linked process");
    return nullptr;
  }
  // A null path asks for a handle to the main program.
  if (path == nullptr)
    return dl::handle_for(0);

  for (size_t i = 0; i < set.count; ++i) {
    if (same_name(set.modules[i].name(), path) ||
        same_name(set.modules[i].soname(), path)) {
      ++set.references[i];
      return dl::handle_for(i);
    }
  }

  if (set.count >= set.capacity) {
    dl::set_error("too many shared objects are open");
    return nullptr;
  }

  const char *library_path =
      elf::library_path_from(reinterpret_cast<char **>(environ));

  const elf::Module *caller_module = nullptr;
  for (size_t i = 0; i < set.count; ++i) {
    if (set.modules[i].contains(caller)) {
      caller_module = &set.modules[i];
      break;
    }
  }

  auto loaded =
      elf::find_and_load(path, caller_module, library_path, set.page_size);
  if (!loaded.has_value()) {
    dl::set_error("cannot open shared object");
    return nullptr;
  }

  const size_t first = set.count;
  set.modules[first] = loaded.value().module;
  set.mappings[first] = loaded.value().mapping;
  // Nothing loaded now has a place in the block each thread already has, so
  // there is no offset from a thread pointer to record. Each thread takes a
  // block of its own for it, the first time it asks.
  set.tls_offsets[first] = 0;
  set.references[first] = 1;
  set.count = first + 1;

  // What the object was linked against has to be there too, and so does what
  // that was linked against. Anything already loaded is left where it is and
  // simply referred to again.
  bool failed = false;
  const char *missing = nullptr;
  for (size_t i = first; i < set.count && !failed; ++i) {
    const elf::Module requester = set.modules[i];
    requester.for_each_needed([&](const char *name) {
      if (failed || already_loaded(set, name))
        return;
      if (set.count >= set.capacity) {
        failed = true;
        missing = name;
        return;
      }
      auto dependency =
          elf::find_and_load(name, &requester, library_path, set.page_size);
      if (!dependency.has_value()) {
        failed = true;
        missing = name;
        return;
      }
      const size_t at = set.count;
      set.modules[at] = dependency.value().module;
      set.mappings[at] = dependency.value().mapping;
      set.tls_offsets[at] = 0;
      set.references[at] = 1;
      set.count = at + 1;
    });
  }

  // Bind against everything now loaded, the new objects included, so they can
  // refer to themselves and to each other as well as to what was there.
  elf::BindResult result;
  if (!failed) {
    elf::SearchOrder order(set.modules, set.count);
    order.set_tls_offsets(set.tls_offsets);
    for (size_t i = first; i < set.count; ++i) {
      const elf::BindResult one = elf::bind_module(set.modules[i], i, order);
      result.bound += one.bound;
      result.unresolved += one.unresolved;
    }
  }

  if (failed || result.unresolved != 0) {
    for (size_t i = set.count; i > first; --i)
      elf::unmap_module(set.mappings[i - 1]);
    set.count = first;
    ++set.generation;
    dl::set_error(failed ? "cannot open a shared object it was linked against"
                         : "shared object has undefined symbols");
    (void)missing;
    return nullptr;
  }

  // What any thread kept for a module that used to be at one of these indices
  // is no longer anything to do with what is there now.
  ++set.generation;

  // Dependencies are initialised before what needs them, so the new modules
  // are run backwards.
  for (size_t i = set.count; i > first; --i)
    run_init_array(set.modules[i - 1]);
  return dl::handle_for(first);
}

} // namespace LIBC_NAMESPACE_DECL
