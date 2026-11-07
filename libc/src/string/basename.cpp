//===-- Implementation of the GNU basename --------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/string/basename.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/string/string_utils.h"

namespace LIBC_NAMESPACE_DECL {

// Unlike the POSIX version this never changes its argument, and a path ending
// in a slash has an empty last component.
LLVM_LIBC_FUNCTION(char *, basename, (const char *path)) {
  const char *last_slash = internal::strrchr_implementation(path, '/');
  return const_cast<char *>(last_slash ? last_slash + 1 : path);
}

} // namespace LIBC_NAMESPACE_DECL
