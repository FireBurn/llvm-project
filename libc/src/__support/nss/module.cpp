//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the loader of service modules.
///
//===----------------------------------------------------------------------===//

#include "src/__support/nss/module.h"

#include "src/__support/macros/config.h"
#include "src/__support/threads/raw_mutex.h"
#include "src/dlfcn/dlopen.h"
#include "src/dlfcn/dlsym.h"

#include <dlfcn.h>

namespace LIBC_NAMESPACE_DECL {
namespace nss {

namespace {

RawMutex lock;
Resolver resolver = nullptr;

constexpr size_t NAME_LIMIT = 4096;

struct Name {
  char text[NAME_LIMIT];
  size_t length = 0;
  bool overflow = false;

  void add(const char *piece) {
    for (; *piece != '\0'; ++piece) {
      if (length + 1 >= NAME_LIMIT) {
        overflow = true;
        return;
      }
      text[length++] = *piece;
    }
    text[length] = '\0';
  }
};

// A service is named on a line of a file anyone may be asked to read, and it
// becomes part of a path, so it may not climb out of the library directories.
bool plain(const char *name) {
  if (name[0] == '\0')
    return false;
  for (const char *p = name; *p != '\0'; ++p)
    if (*p == '/')
      return false;
  return true;
}

} // namespace

void *service_function(Source &source, const char *function) {
  lock.lock(cpp::nullopt, false);
  Resolver stand_in = resolver;
  lock.unlock();
  if (stand_in != nullptr)
    return stand_in(source.name, function);

  if (!plain(source.name))
    return nullptr;

  lock.lock(cpp::nullopt, false);
  void *module = source.module;
  bool tried = source.module_tried;
  lock.unlock();

  if (!tried) {
    Name library;
    library.add("libnss_");
    library.add(source.name);
    library.add(".so.2");
    // Opened with the lock released: a module may look something up as it
    // is loaded.
    if (!library.overflow)
      module = internal::dlopen_impl(library.text, RTLD_LAZY);
    lock.lock(cpp::nullopt, false);
    source.module = module;
    source.module_tried = true;
    lock.unlock();
  }
  if (module == nullptr)
    return nullptr;

  Name symbol;
  symbol.add("_nss_");
  symbol.add(source.name);
  symbol.add("_");
  symbol.add(function);
  if (symbol.overflow)
    return nullptr;
  return internal::dlsym_impl(module, symbol.text);
}

void TESTONLY_set_resolver(Resolver fn) {
  lock.lock(cpp::nullopt, false);
  resolver = fn;
  lock.unlock();
}

} // namespace nss
} // namespace LIBC_NAMESPACE_DECL
