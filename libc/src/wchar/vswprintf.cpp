//===-- Implementation of vswprintf ---------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/wchar/vswprintf.h"

#include "hdr/errno_macros.h"
#include "hdr/func/free.h"
#include "hdr/types/size_t.h"
#include "hdr/types/wchar_t.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/wchar/wide_format.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, vswprintf,
                   (wchar_t *__restrict buffer, size_t bufsz,
                    const wchar_t *__restrict format, va_list vlist)) {
  if (format == nullptr || (buffer == nullptr && bufsz != 0)) {
    libc_errno = EINVAL;
    return -1;
  }

  wchar_t *wide = nullptr;
  int written = internal::format_wide(format, vlist, &wide);
  if (written < 0)
    return -1;

  // Unlike snprintf, this reports failure rather than the length it wanted
  // when the result does not fit.
  if (bufsz == 0 || static_cast<size_t>(written) >= bufsz) {
    if (bufsz != 0)
      buffer[bufsz - 1] = L'\0';
    free(wide);
    return -1;
  }

  for (int i = 0; i <= written; ++i)
    buffer[i] = wide[i];
  free(wide);
  return written;
}

} // namespace LIBC_NAMESPACE_DECL
