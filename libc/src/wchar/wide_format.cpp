//===-- Formatting for the wide printf family -----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// A wide format asks for the same conversions the narrow printf performs,
// and its directives are all in the portable character set, so the format is
// narrowed and the narrow printf does the work. `%s` takes a char * in both,
// `%ls` a wchar_t * and `%lc` a wint_t. A format character outside ASCII is
// refused rather than guessed at.
//
//===----------------------------------------------------------------------===//

#include "src/wchar/wide_format.h"

#include "hdr/errno_macros.h"
#include "hdr/func/free.h"
#include "hdr/func/malloc.h"
#include "hdr/types/size_t.h"
#include "src/__support/libc_errno.h"
#include "src/__support/wchar/mbrtowc.h"
#include "src/__support/wchar/mbstate.h"
#include "src/stdio/vsnprintf.h"

namespace LIBC_NAMESPACE_DECL {
namespace internal {

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

int format_wide(const wchar_t *format, va_list vlist, wchar_t **out) {
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
  int written = LIBC_NAMESPACE::vsnprintf(text, static_cast<size_t>(len) + 1,
                                          narrow, vlist);
  free(narrow);
  if (written < 0) {
    free(text);
    return -1;
  }

  // %lc and %ls come out as multibyte sequences, so the text is decoded
  // rather than widened a byte at a time. It never holds more characters than
  // bytes.
  auto *wide = reinterpret_cast<wchar_t *>(
      malloc((static_cast<size_t>(written) + 1) * sizeof(wchar_t)));
  if (wide == nullptr) {
    free(text);
    libc_errno = ENOMEM;
    return -1;
  }
  mbstate state;
  size_t in = 0;
  int count = 0;
  while (in < static_cast<size_t>(written)) {
    auto used = mbrtowc(&wide[count], text + in,
                        static_cast<size_t>(written) - in, &state);
    // An embedded null from %c counts as one character, like any other.
    if (!used.has_value() || used.value() > static_cast<size_t>(written)) {
      free(text);
      free(wide);
      libc_errno = EILSEQ;
      return -1;
    }
    in += used.value() == 0 ? 1 : used.value();
    ++count;
  }
  wide[count] = L'\0';
  free(text);
  *out = wide;
  return count;
}

} // namespace internal
} // namespace LIBC_NAMESPACE_DECL
