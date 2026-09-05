//===-- Implementation of fgetpwent ---------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/pwd/fgetpwent.h"

#include "hdr/errno_macros.h"
#include "hdr/types/FILE.h"
#include "hdr/types/struct_passwd.h"
#include "src/__support/CPP/span.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/pwd/pwd_utils.h"
#include "src/stdio/fgets.h"

namespace LIBC_NAMESPACE_DECL {

namespace {
// The entry the last call returned, which the next one overwrites.
struct passwd entry;
char line[pwd::PASSWD_LINE_SIZE];
} // anonymous namespace

// Reads one entry from a stream the caller opened, rather than from the
// password file. Blank lines and comments are passed over.
LLVM_LIBC_FUNCTION(struct passwd *, fgetpwent, (::FILE * stream)) {
  if (stream == nullptr) {
    libc_errno = EINVAL;
    return nullptr;
  }

  for (;;) {
    if (LIBC_NAMESPACE::fgets(line, sizeof(line), stream) == nullptr)
      return nullptr; // The end of the file is not an error to report.

    size_t len = 0;
    while (line[len] != '\0')
      ++len;
    if (len > 0 && line[len - 1] == '\n')
      line[--len] = '\0';

    if (len == 0 || line[0] == '#')
      continue;

    if (!internal::parse_line(cpp::span<char>(line, len + 1), &entry)) {
      libc_errno = EINVAL;
      return nullptr;
    }
    return &entry;
  }
}

} // namespace LIBC_NAMESPACE_DECL
