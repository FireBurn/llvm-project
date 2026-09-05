//===-- Linux implementation of mkfifo ------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/sys/stat/mkfifo.h"

#include "hdr/fcntl_macros.h"
#include "hdr/types/mode_t.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/sys/stat/mkfifoat.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, mkfifo, (const char *path, mode_t mode)) {
  return LIBC_NAMESPACE::mkfifoat(AT_FDCWD, path, mode);
}

} // namespace LIBC_NAMESPACE_DECL
