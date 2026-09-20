//===-- Implementation of vswprintf ---------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// The conversions a wide format asks for are the same ones the narrow
// printf already performs, and in the C locale, which is the only one this
// library serves, a wide character and the byte it came from are the same
// value below 0x80. So the format is narrowed, the narrow printf does the
// work, and the result is widened back.
//
// The specifiers line up: `%s` takes a char * in both, `%ls` a wchar_t * in
// both, and `%lc` a wint_t. A byte above 0x7f has no single-character
// meaning in this locale and is refused rather than guessed at.
//
//===----------------------------------------------------------------------===//

#include "src/wchar/vswprintf.h"

#include "hdr/func/free.h"
#include "hdr/func/malloc.h"
#include "hdr/types/size_t.h"
#include "hdr/types/wchar_t.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/stdio/vsnprintf.h"

namespace LIBC_NAMESPACE_DECL {

namespace {

// The format, with each character taken down to the byte it stands for.
// Returns nothing when a character has no such byte.
char *narrow_format(const wchar_t *format) {
  size_t len = 0;
  while (format[len] != L'\0')
    ++len;

  char *out = reinterpret_cast<char *>(malloc(len + 1));
  if (out == nullptr)
    return nullptr;

  for (size_t i = 0; i < len; ++i) {
    if (format[i] < 0 || format[i] > 0x7f) {
      free(out);
      return nullptr;
    }
    out[i] = static_cast<char>(format[i]);
  }
  out[len] = '\0';
  return out;
}

} // anonymous namespace

LLVM_LIBC_FUNCTION(int, vswprintf,
                   (wchar_t *__restrict buffer, size_t bufsz,
                    const wchar_t *__restrict format, va_list vlist)) {
  if (format == nullptr || (buffer == nullptr && bufsz != 0)) {
    libc_errno = EINVAL;
    return -1;
  }

  char *narrow = narrow_format(format);
  if (narrow == nullptr) {
    libc_errno = EILSEQ;
    return -1;
  }

  // How long the result comes to, before writing any of it.
  va_list measure;
  va_copy(measure, vlist);
  int len = LIBC_NAMESPACE::vsnprintf(nullptr, 0, narrow, measure);
  va_end(measure);
  if (len < 0) {
    free(narrow);
    return -1;
  }

  char *text = reinterpret_cast<char *>(malloc(static_cast<size_t>(len) + 1));
  if (text == nullptr) {
    free(narrow);
    libc_errno = ENOMEM;
    return -1;
  }

  int written =
      LIBC_NAMESPACE::vsnprintf(text, static_cast<size_t>(len) + 1, narrow,
                                vlist);
  free(narrow);
  if (written < 0) {
    free(text);
    return -1;
  }

  // Unlike snprintf, this reports failure rather than the length it wanted
  // when the result does not fit.
  if (bufsz == 0 || static_cast<size_t>(written) >= bufsz) {
    if (bufsz != 0)
      buffer[bufsz - 1] = L'\0';
    free(text);
    return -1;
  }

  for (int i = 0; i < written; ++i)
    buffer[i] = static_cast<wchar_t>(static_cast<unsigned char>(text[i]));
  buffer[written] = L'\0';

  free(text);
  return written;
}

} // namespace LIBC_NAMESPACE_DECL
