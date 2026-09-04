//===-- Implementation of setmntent
//----------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/mntent/setmntent.h"

#include "hdr/types/FILE.h"
#include "src/__support/File/file.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(::FILE *, setmntent,
                   (const char *filename, const char *mode)) {
  if (filename == nullptr || mode == nullptr) {
    libc_errno = EINVAL;
    return nullptr;
  }
  auto result = LIBC_NAMESPACE::openfile(filename, mode);
  if (!result.has_value()) {
    libc_errno = result.error();
    return nullptr;
  }
  return reinterpret_cast<::FILE *>(result.value());
}

} // namespace LIBC_NAMESPACE_DECL
