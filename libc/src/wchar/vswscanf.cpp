//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of vswscanf
///
//===----------------------------------------------------------------------===//
//
// The format is narrowed and the text encoded, and the narrow scanf does the
// work, as for the wide printf family. `%s` stores a char *, `%ls` a wchar_t *.
// A field width, and the count %n gives, are in bytes of the encoded text,
// which is the same as characters for ASCII. A format character outside ASCII
// is refused rather than guessed at.
//
//===----------------------------------------------------------------------===//

#include "src/wchar/vswscanf.h"

#include "hdr/errno_macros.h"
#include "hdr/func/free.h"
#include "hdr/stdio_macros.h"
#include "hdr/types/wchar_t.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/stdio/vsscanf.h"
#include "src/wchar/wide_format.h"

#include <stdarg.h>

namespace LIBC_NAMESPACE_DECL {

namespace internal {

int vswscanf_impl(const wchar_t *__restrict buffer,
                  const wchar_t *__restrict format, va_list vlist) {
  if (buffer == nullptr || format == nullptr) {
    libc_errno = EINVAL;
    return EOF;
  }

  char *narrow = internal::narrow_format(format);
  if (narrow == nullptr) {
    libc_errno = EILSEQ;
    return EOF;
  }
  char *text = internal::encode_text(buffer);
  if (text == nullptr) {
    free(narrow);
    return EOF;
  }

  int ret = internal::vsscanf_impl(text, narrow, vlist);
  free(text);
  free(narrow);
  return ret;
}

} // namespace internal

LLVM_LIBC_FUNCTION(int, vswscanf,
                   (const wchar_t *__restrict buffer,
                    const wchar_t *__restrict format, va_list vlist)) {
  return internal::vswscanf_impl(buffer, format, vlist);
}

} // namespace LIBC_NAMESPACE_DECL
