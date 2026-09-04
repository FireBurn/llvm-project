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

// Recursive, because the initialisers of what was just loaded run while it
// is held, and an initialiser is entitled to call back into the loader:
// dlopen for something it needs, or dlsym for something it was given. A
// plain mutex leaves the one thread waiting on itself, which is what
// systemd did as PID 1, with nothing left to wake it. glibc's load lock is
// recursive for the same reason.
Mutex dl_mutex(false, /*is_recursive=*/true, false, false);
const char *dl_last_error = nullptr;

} // namespace dl
} // namespace LIBC_NAMESPACE_DECL
