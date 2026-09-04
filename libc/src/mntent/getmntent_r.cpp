//===-- Implementation of getmntent_r
//----------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/mntent/getmntent_r.h"

#include "hdr/types/FILE.h"
#include "src/__support/File/file.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/mntent/mntent_utils.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(struct mntent *, getmntent_r,
                   (::FILE *__restrict stream, struct mntent *__restrict entry,
                    char *__restrict buffer, int bufsize)) {
  if (stream == nullptr || entry == nullptr || buffer == nullptr ||
      bufsize <= 0) {
    libc_errno = EINVAL;
    return nullptr;
  }

  auto *file = reinterpret_cast<File *>(stream);
  File::FileLock lock(file);

  // Blank and comment lines are skipped rather than reported, so keep
  // reading until a line parses or the file runs out.
  for (;;) {
    size_t len = 0;
    bool saw_newline = false;
    for (; len < static_cast<size_t>(bufsize) - 1; ++len) {
      auto result = file->read_unlocked(&buffer[len], 1);
      if (result.has_error()) {
        libc_errno = result.error;
        return nullptr;
      }
      if (result.value != 1)
        break;
      if (buffer[len] == '\n') {
        saw_newline = true;
        break;
      }
    }
    buffer[len] = '\0';

    // A line too long for the buffer would be parsed as two, so drop the
    // rest of it instead.
    if (!saw_newline && len == static_cast<size_t>(bufsize) - 1) {
      char c = '\0';
      for (;;) {
        auto result = file->read_unlocked(&c, 1);
        if (result.has_error() || result.value != 1 || c == '\n')
          break;
      }
    }

    if (len == 0 && !saw_newline)
      return nullptr; // End of file.

    if (mntent::parse_line(buffer, entry))
      return entry;
  }
}

} // namespace LIBC_NAMESPACE_DECL
