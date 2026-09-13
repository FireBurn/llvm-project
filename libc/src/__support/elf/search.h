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
#include "hdr/fcntl_macros.h"
#include "hdr/link_macros.h"
#include "hdr/types/size_t.h"
#include "src/__support/CPP/optional.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/close.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/open.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/read.h"
#include "src/__support/elf/load_module.h"
#include "src/__support/elf/module.h"
#include "src/__support/elf/run_path.h"
#include "src/__support/macros/attributes.h"
#include "src/__support/macros/config.h"
#include "src/__support/macros/properties/architectures.h"

namespace LIBC_NAMESPACE_DECL {
namespace elf {

// Where a bare soname is looked for once nothing else has it. There is no
// cache of what is installed: musl manages without one and a format compatible
// with glibc's would buy nothing here.
constexpr const char *DEFAULT_SEARCH_PATHS[] = {"/lib", "/usr/lib"};

// Where a system lists the other directories it keeps libraries in, looked at
// before the default ones. It is named after the loader, as musl names its
// own, and a system that already keeps glibc's ld.so.conf can make it a link
// to that.
#if defined(LIBC_TARGET_ARCH_IS_X86_64)
constexpr const char *SYSTEM_PATH_FILE = "/etc/ld-llvm-libc-x86_64.path";
#elif defined(LIBC_TARGET_ARCH_IS_AARCH64)
constexpr const char *SYSTEM_PATH_FILE = "/etc/ld-llvm-libc-aarch64.path";
#elif defined(LIBC_TARGET_ARCH_IS_ANY_RISCV)
constexpr const char *SYSTEM_PATH_FILE = "/etc/ld-llvm-libc-riscv.path";
#elif defined(LIBC_TARGET_ARCH_IS_ARM)
constexpr const char *SYSTEM_PATH_FILE = "/etc/ld-llvm-libc-arm.path";
#else
constexpr const char *SYSTEM_PATH_FILE = nullptr;
#endif

// How much of that file is read, and how long the list made from it may be.
constexpr size_t MAX_SYSTEM_PATHS = 4096;

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

// Turns the text of a path file into a colon separated list. Directories are
// separated by newlines or colons, and a # starts a comment that runs to the
// end of its line, which lets the file be glibc's ld.so.conf as well as one
// written the way musl's is. Blanks around a directory are dropped. Returns
// the length of the list, which ends at the last whole directory that fits.
LIBC_INLINE size_t path_list_from_text(const char *text, size_t length,
                                       char *out, size_t capacity) {
  if (capacity == 0)
    return 0;
  size_t n = 0;
  size_t i = 0;
  while (i < length) {
    const char c = text[i];
    if (c == '\n' || c == ':' || c == ' ' || c == '\t' || c == '\r') {
      ++i;
      continue;
    }
    if (c == '#') {
      while (i < length && text[i] != '\n')
        ++i;
      continue;
    }
    const size_t start = i;
    while (i < length && text[i] != '\n' && text[i] != ':' && text[i] != '#')
      ++i;
    size_t end = i;
    while (end > start && (text[end - 1] == ' ' || text[end - 1] == '\t' ||
                           text[end - 1] == '\r'))
      --end;
    const size_t separator = n > 0 ? 1 : 0;
    if (n + separator + (end - start) + 1 > capacity)
      break;
    if (separator)
      out[n++] = ':';
    for (size_t k = start; k < end; ++k)
      out[n++] = text[k];
  }
  out[n] = '\0';
  return n;
}

// The directories a system's path file lists. The file is read the first time
// a search gets as far as needing them, so a process whose libraries are all
// found sooner never opens it.
class SystemPaths {
public:
  LIBC_INLINE SystemPaths(const char *file = SYSTEM_PATH_FILE) : file_(file) {}

  // The list, or null where there is no file or nothing in it.
  LIBC_INLINE const char *list() {
    if (!read_) {
      read_ = true;
      load();
    }
    return length_ == 0 ? nullptr : list_;
  }

private:
  LIBC_INLINE void load() {
    if (file_ == nullptr)
      return;
    auto fd = linux_syscalls::open(file_, O_RDONLY | O_CLOEXEC, 0);
    if (!fd.has_value())
      return;
    char text[MAX_SYSTEM_PATHS];
    size_t got = 0;
    while (got < sizeof(text)) {
      auto n = linux_syscalls::read(fd.value(), text + got, sizeof(text) - got);
      if (!n.has_value() || n.value() <= 0)
        break;
      got += static_cast<size_t>(n.value());
    }
    linux_syscalls::close(fd.value());
    // A file longer than was read is cut at its last whole line, so that no
    // directory is taken from the part of a name that fitted.
    if (got == sizeof(text))
      while (got > 0 && text[got - 1] != '\n')
        --got;
    length_ = path_list_from_text(text, got, list_, sizeof(list_));
  }

  const char *file_;
  bool read_ = false;
  size_t length_ = 0;
  char list_[MAX_SYSTEM_PATHS];
};

// The older DT_RPATH, which is looked at before LD_LIBRARY_PATH and so cannot
// be overridden. A module carrying DT_RUNPATH is not using DT_RPATH at all,
// even where both are present, so the newer one turns the older one off.
LIBC_INLINE const char *rpath_of(const Module &module) {
  const char *strings = module.strtab();
  if (strings == nullptr)
    return nullptr;
  if (module.dynamic().value(DT_RUNPATH))
    return nullptr;
  if (auto offset = module.dynamic().value(DT_RPATH))
    return strings + *offset;
  return nullptr;
}

// DT_RUNPATH, which is looked at after LD_LIBRARY_PATH. Being overridable is
// the whole reason it was added: a program built against libraries that are
// not installed yet is run with LD_LIBRARY_PATH naming where they actually
// are, and that has to win over the path recorded for where they will end up.
LIBC_INLINE const char *runpath_of(const Module &module) {
  const char *strings = module.strtab();
  if (strings == nullptr)
    return nullptr;
  if (auto offset = module.dynamic().value(DT_RUNPATH))
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
// looked for under the asking object's DT_RPATH, then under `library_path`,
// then under its DT_RUNPATH, then under what the system's path file lists,
// then under the default directories. The two run paths sit on either side of
// LD_LIBRARY_PATH, which is what tells them apart.
//
// `requester` is the module whose run path applies, or null where none does.
// `origin` is what $ORIGIN in that run path stands for, or null for the
// directory of the requester's own name. `system_paths` is null where no path
// file is to be read.
LIBC_INLINE cpp::optional<LoadedModule>
find_and_load(const char *name, const Module *requester, const char *origin,
              const char *library_path, SystemPaths *system_paths,
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

  if (requester != nullptr && origin == nullptr)
    origin = requester->name();
  if (requester != nullptr) {
    auto loaded =
        load_from_path_list(rpath_of(*requester), name, page_size, origin);
    if (loaded.has_value())
      return loaded;
  }
  if (auto loaded = load_from_path_list(library_path, name, page_size))
    return loaded;
  if (requester != nullptr) {
    auto loaded =
        load_from_path_list(runpath_of(*requester), name, page_size, origin);
    if (loaded.has_value())
      return loaded;
  }
  if (system_paths != nullptr) {
    auto loaded = load_from_path_list(system_paths->list(), name, page_size);
    if (loaded.has_value())
      return loaded;
  }
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
LIBC_INLINE const char *value_of(char **envp, const char *key) {
  if (envp == nullptr)
    return nullptr;
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

LIBC_INLINE const char *library_path_from(char **envp) {
  return value_of(envp, "LD_LIBRARY_PATH=");
}

// LD_PRELOAD, the objects to load before anything the program asked for so
// that what they define is found first.
LIBC_INLINE const char *preload_list_from(char **envp) {
  return value_of(envp, "LD_PRELOAD=");
}

} // namespace elf
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC___SUPPORT_ELF_SEARCH_H
