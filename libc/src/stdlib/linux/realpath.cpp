//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of POSIX realpath.
///
//===----------------------------------------------------------------------===//

#include "src/stdlib/realpath.h"
#include "src/__support/OSUtil/linux/realpath.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(char *, realpath,
                   (const char *__restrict path,
                    char *__restrict resolved_path)) {
  ErrorOr<char *> res = internal::realpath_impl(path, resolved_path);
  if (!res) {
    libc_errno = res.error();
    return nullptr;
  }
  return *res;
}

} // namespace LIBC_NAMESPACE_DECL
