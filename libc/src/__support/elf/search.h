//===-- Finding a shared object by name -------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC___SUPPORT_ELF_SEARCH_H
#define LLVM_LIBC_SRC___SUPPORT_ELF_SEARCH_H

#include "hdr/elf_macros.h"
#include "hdr/link_macros.h"
#include "hdr/types/size_t.h"
#include "src/__support/CPP/optional.h"
#include "src/__support/elf/load_module.h"
#include "src/__support/elf/module.h"
#include "src/__support/elf/run_path.h"
#include "src/__support/macros/attributes.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {
namespace elf {

// Where a bare soname is looked for. There is no cache of what is installed:
// musl manages without one and a format compatible with glibc's would buy
// nothing here.
constexpr const char *DEFAULT_SEARCH_PATHS[] = {"/lib", "/usr/lib"};

// The longest path this will build. A name longer than this is one nothing
// can open anyway.
constexpr size_t MAX_SEARCH_PATH = 256;

// Joins a directory and a name with a separator between them. Returns false
// if the result would not fit.
LIBC_INLINE bool join_path(const char *dir, const char *name, char *out,
                           size_t capacity) {
  size_t n = 0;
  for (const char *p = dir; *p != '\0'; ++p) {
    if (n + 2 >= capacity)
      return false;
    out[n++] = *p;
  }
  out[n++] = '/';
  for (const char *p = name; *p != '\0'; ++p) {
    if (n + 1 >= capacity)
      return false;
    out[n++] = *p;
  }
  out[n] = '\0';
  return true;
}

// DT_RUNPATH, or the older DT_RPATH when a module still carries one.
LIBC_INLINE const char *run_path_of(const Module &module) {
  const char *strings = module.strtab();
  if (strings == nullptr)
    return nullptr;
  if (auto offset = module.dynamic().value(DT_RUNPATH))
    return strings + *offset;
  if (auto offset = module.dynamic().value(DT_RPATH))
    return strings + *offset;
  return nullptr;
}

// Tries `name` under each colon separated directory in `list`. `origin` is
// what $ORIGIN stands for in that list, and is null where the list may not
// use it.
LIBC_INLINE cpp::optional<LoadedModule>
load_from_path_list(const char *list, const char *name, size_t page_size,
                    const char *origin = nullptr) {
  if (list == nullptr)
    return cpp::nullopt;
  for (const char *segment = list; segment != nullptr;) {
    const char *end = segment;
    while (*end != '\0' && *end != ':')
      ++end;
    char dir[MAX_SEARCH_PATH];
    const size_t length = static_cast<size_t>(end - segment);
    if (length > 0 &&
        expand_run_path(segment, length, origin, dir, sizeof(dir))) {
      char path[MAX_SEARCH_PATH];
      if (join_path(dir, name, path, sizeof(path))) {
        auto loaded = load_module(path, page_size);
        if (loaded.has_value())
          return loaded.value();
      }
    }
    segment = (*end == '\0') ? nullptr : end + 1;
  }
  return cpp::nullopt;
}

// Loads the object `name` stands for, in the order a loader is expected to
// look: a name with a directory in it is taken as it is, and a bare one is
// looked for under the run path of the object asking, then under
// `library_path`, then under the system directories.
//
// `requester` is the module whose run path applies, or null where none does.
LIBC_INLINE cpp::optional<LoadedModule> find_and_load(const char *name,
                                                      const Module *requester,
                                                      const char *library_path,
                                                      size_t page_size) {
  if (name == nullptr)
    return cpp::nullopt;

  for (const char *p = name; *p != '\0'; ++p)
    if (*p == '/') {
      auto loaded = load_module(name, page_size);
      if (loaded.has_value())
        return loaded.value();
      return cpp::nullopt;
    }

  if (requester != nullptr) {
    auto loaded = load_from_path_list(run_path_of(*requester), name, page_size,
                                      requester->name());
    if (loaded.has_value())
      return loaded;
  }
  if (auto loaded = load_from_path_list(library_path, name, page_size))
    return loaded;
  for (const char *dir : DEFAULT_SEARCH_PATHS) {
    char path[MAX_SEARCH_PATH];
    if (!join_path(dir, name, path, sizeof(path)))
      continue;
    auto loaded = load_module(path, page_size);
    if (loaded.has_value())
      return loaded.value();
  }
  return cpp::nullopt;
}

// LD_LIBRARY_PATH, without the value being copied anywhere: the environment
// outlives whoever looks at it.
LIBC_INLINE const char *library_path_from(char **envp) {
  if (envp == nullptr)
    return nullptr;
  const char key[] = "LD_LIBRARY_PATH=";
  for (char **e = envp; *e != nullptr; ++e) {
    const char *p = *e;
    size_t i = 0;
    for (; key[i] != '\0' && p[i] == key[i]; ++i)
      ;
    if (key[i] == '\0')
      return p + i;
  }
  return nullptr;
}

} // namespace elf
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC___SUPPORT_ELF_SEARCH_H
