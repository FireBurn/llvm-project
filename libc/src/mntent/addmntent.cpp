//===-- Implementation of addmntent ---------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/mntent/addmntent.h"

#include "hdr/stdio_macros.h"
#include "hdr/types/FILE.h"
#include "src/__support/File/file.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/mntent/mntent_utils.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, addmntent,
                   (::FILE *__restrict stream,
                    const struct mntent *__restrict entry)) {
  if (stream == nullptr || entry == nullptr) {
    libc_errno = EINVAL;
    return 1;
  }

  auto *file = reinterpret_cast<File *>(stream);

  // The entry is appended, so a stream also open for reading has to be put
  // at the end first. seek takes the file lock itself, so it has to happen
  // before the lock the write is made under.
  if (!file->seek(0, SEEK_END).has_value())
    return 1;

  File::FileLock lock(file);
  if (!mntent::write_entry(file, entry))
    return 1;
  if (file->flush_unlocked() != 0)
    return 1;
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL
