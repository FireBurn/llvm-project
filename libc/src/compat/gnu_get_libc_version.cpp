//===-- Implementation of gnu_get_libc_version ----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/compat/gnu_get_libc_version.h"

#include "src/__support/common.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

// Callers ask this to find out which of the platform's calls they may use,
// not who wrote the library. The number states the level this libc answers
// to, which is the one where the threading, dynamic linking and timer calls
// all live in libc itself rather than in libraries of their own, and where
// close_range, statx, getrandom, copy_file_range and a posix_spawn that says
// why a child could not run are all there. Every one of those is here.
LLVM_LIBC_FUNCTION(const char *, gnu_get_libc_version, (void)) {
  return "2.34";
}

// glibc reports how its own release was made. Nothing reads it to decide
// anything, and there is nothing truthful to say, so it says so.
LLVM_LIBC_FUNCTION(const char *, gnu_get_libc_release, (void)) {
  return "stable";
}

} // namespace LIBC_NAMESPACE_DECL
