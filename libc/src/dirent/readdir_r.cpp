//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of readdir_r
///
//===----------------------------------------------------------------------===//

#include "readdir_r.h"

#include "src/__support/File/dir.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"

#include <dirent.h>

namespace LIBC_NAMESPACE_DECL {

// The entry is copied into the caller's, so that a thread other than the one
// that holds the directory can keep it, which is what the function is for.
// A caller's entry is as large as the largest the library returns.
LLVM_LIBC_FUNCTION(int, readdir_r,
                   (::DIR * dir, struct ::dirent *entry,
                    struct ::dirent **result)) {
  auto *d = reinterpret_cast<LIBC_NAMESPACE::Dir *>(dir);
  auto dirent_val = d->read();
  if (!dirent_val) {
    *result = nullptr;
    return dirent_val.error();
  }
  struct ::dirent *found = dirent_val.value();
  if (found == nullptr) {
    *result = nullptr;
    return 0;
  }
  const char *from = reinterpret_cast<const char *>(found);
  char *to = reinterpret_cast<char *>(entry);
  unsigned long length = found->d_reclen;
  if (length > sizeof(struct ::dirent))
    length = sizeof(struct ::dirent);
  for (unsigned long i = 0; i < length; ++i)
    to[i] = from[i];
  *result = entry;
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL
