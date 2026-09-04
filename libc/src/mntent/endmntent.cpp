//===-- Implementation of endmntent
//----------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/mntent/endmntent.h"

#include "hdr/types/FILE.h"
#include "src/__support/File/file.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

// Unlike fclose, endmntent returns 1 rather than 0, and always.
LLVM_LIBC_FUNCTION(int, endmntent, (::FILE * stream)) {
  if (stream != nullptr)
    reinterpret_cast<File *>(stream)->close();
  return 1;
}

} // namespace LIBC_NAMESPACE_DECL
