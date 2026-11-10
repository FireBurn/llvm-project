//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of ns_makecanon
///
//===----------------------------------------------------------------------===//

#include "src/arpa/nameser/ns_makecanon.h"

#include "hdr/errno_macros.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, ns_makecanon,
                   (const char *src, char *dst, size_t dstsiz)) {
  // The name with one dot at its end, unless that dot is escaped.
  if (src == nullptr || dst == nullptr) {
    libc_errno = EMSGSIZE;
    return -1;
  }
  size_t n = 0;
  while (src[n] != '\0')
    ++n;
  if (n + 2 > dstsiz) {
    libc_errno = EMSGSIZE;
    return -1;
  }
  for (size_t i = 0; i < n; ++i)
    dst[i] = src[i];
  size_t slashes = 0;
  if (n > 0)
    for (size_t i = n - 1; i > 0 && src[i - 1] == '\\'; --i)
      ++slashes;
  if (n == 0 || src[n - 1] != '.' || slashes % 2 != 0)
    dst[n++] = '.';
  dst[n] = '\0';
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL
