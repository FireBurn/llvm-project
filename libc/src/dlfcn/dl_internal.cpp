//===-- Shared state of the dlfcn functions -------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/dlfcn/dl_internal.h"

#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {
namespace dl {

Mutex dl_mutex(false, false, false, false);
const char *dl_last_error = nullptr;

} // namespace dl
} // namespace LIBC_NAMESPACE_DECL
