//===-- Shared state of the dlfcn functions ---------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_DLFCN_DL_INTERNAL_H
#define LLVM_LIBC_SRC_DLFCN_DL_INTERNAL_H

#include "hdr/types/size_t.h"
#include "src/__support/elf/passive_abi.h"
#include "src/__support/macros/attributes.h"
#include "src/__support/macros/config.h"
#include "src/__support/threads/mutex.h"

namespace LIBC_NAMESPACE_DECL {
namespace dl {

// A handle is the module's index plus one, so that it is never null and
// carries no pointer the caller could dereference.
LIBC_INLINE void *handle_for(size_t index) {
  return reinterpret_cast<void *>(index + 1);
}

// Returns false when the handle did not come from dlopen.
LIBC_INLINE bool index_for(void *handle, size_t &index) {
  const size_t value = reinterpret_cast<size_t>(handle);
  if (value == 0 || value > elf::loaded_modules().count)
    return false;
  index = value - 1;
  return true;
}

extern Mutex dl_mutex;

// The message dlerror will report, or null when there is nothing to report.
// Only string literals are stored, so there is nothing to own.
extern const char *dl_last_error;

LIBC_INLINE void set_error(const char *message) { dl_last_error = message; }

} // namespace dl
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_DLFCN_DL_INTERNAL_H
