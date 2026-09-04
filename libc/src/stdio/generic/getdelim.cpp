//===-- Implementation of getdelim ----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/stdio/getdelim.h"
#include "src/__support/File/file.h"

#include "hdr/errno_macros.h"
#include "hdr/func/realloc.h"
#include "hdr/types/FILE.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

namespace {

// The buffer grows geometrically, starting here when the caller passes none.
constexpr size_t DEFAULT_BUFSIZE = 128;

} // anonymous namespace

LLVM_LIBC_FUNCTION(ssize_t, getdelim,
                   (char **__restrict lineptr, size_t *__restrict n, int delim,
                    ::FILE *__restrict raw_stream)) {
  if (lineptr == nullptr || n == nullptr || raw_stream == nullptr) {
    libc_errno = EINVAL;
    return -1;
  }
  if (*lineptr == nullptr)
    *n = 0;

  auto stream = reinterpret_cast<LIBC_NAMESPACE::File *__restrict>(raw_stream);
  File::FileLock lock(stream);

  size_t len = 0;
  for (;;) {
    // Always keep room for the character just read plus the null terminator.
    if (len + 2 > *n) {
      size_t new_size = *n < DEFAULT_BUFSIZE ? DEFAULT_BUFSIZE : *n * 2;
      char *new_buf = reinterpret_cast<char *>(::realloc(*lineptr, new_size));
      if (new_buf == nullptr) {
        libc_errno = ENOMEM;
        return -1;
      }
      *lineptr = new_buf;
      *n = new_size;
    }

    unsigned char c;
    auto result = stream->read_unlocked(&c, 1);
    if (result.has_error()) {
      libc_errno = result.error;
      return -1;
    }
    if (result.value != 1)
      break;

    (*lineptr)[len++] = static_cast<char>(c);
    if (c == static_cast<unsigned char>(delim))
      break;
  }

  // End of input with nothing read is reported as a failure, without
  // disturbing the caller's buffer.
  if (len == 0)
    return -1;

  (*lineptr)[len] = '\0';
  return static_cast<ssize_t>(len);
}

} // namespace LIBC_NAMESPACE_DECL
