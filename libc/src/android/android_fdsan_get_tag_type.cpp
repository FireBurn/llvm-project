//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of android_fdsan_get_tag_type.
///
//===----------------------------------------------------------------------===//

#include "src/android/android_fdsan_get_tag_type.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(const char *, android_fdsan_get_tag_type, (uint64_t tag)) {
  static constexpr const char *NAMES[] = {
      "generic",
      "FILE*",
      "DIR*",
      "unique_fd",
      "sqlite",
      "FileInputStream",
      "FileOutputStream",
      "RandomAccessFile",
      "ParcelFileDescriptor",
      "ArtFd",
      "DatagramSocketImpl",
      "SocketImpl",
      "ZipArchive",
      "native_handle_t",
      "Parcel",
  };
  uint64_t type = tag >> 56;
  if (type < sizeof(NAMES) / sizeof(NAMES[0]))
    return NAMES[type];
  return "unknown";
}

} // namespace LIBC_NAMESPACE_DECL
