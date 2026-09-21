//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of wcsftime
///
//===----------------------------------------------------------------------===//

#include "src/wchar/wcsftime.h"

#include "hdr/types/size_t.h"
#include "hdr/types/struct_tm.h"
#include "hdr/types/wchar_t.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/__support/macros/null_check.h"
#include "src/__support/printf_core/writer.h"
#include "src/time/strftime_core/strftime_main.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(size_t, wcsftime,
                   (wchar_t *__restrict buffer, size_t buffsz,
                    const wchar_t *__restrict format, const tm *timeptr)) {
  if (buffsz > 0)
    LIBC_CRASH_ON_NULLPTR(buffer);
  LIBC_CRASH_ON_NULLPTR(format);
  LIBC_CRASH_ON_NULLPTR(timeptr);

  // Only a conversion specification is understood by strftime, and every
  // character of one is ASCII, so each is converted on its own and anything
  // else in the format is copied through as it stands.
  size_t out = 0;
  size_t i = 0;
  while (format[i] != L'\0') {
    if (format[i] != L'%') {
      if (out + 1 >= buffsz)
        return 0;
      buffer[out++] = format[i++];
      continue;
    }

    char spec[32];
    size_t spec_len = 0;
    spec[spec_len++] = '%';
    ++i;
    while (format[i] > 0 && format[i] < 128 && spec_len < sizeof(spec) - 1) {
      char c = static_cast<char>(format[i++]);
      spec[spec_len++] = c;
      // E and O modify the conversion that follows them.
      if (c == '%' || ((c | 0x20) >= 'a' && (c | 0x20) <= 'z' && c != 'E' &&
                       c != 'O'))
        break;
    }
    spec[spec_len] = '\0';

    char text[256];
    printf_core::Writer writer =
        printf_core::make_drop_overflow_writer(text, sizeof(text) - 1);
    auto ret = strftime_core::strftime_main(&writer, spec, timeptr);
    if (!ret.has_value() || ret.value() >= sizeof(text))
      return 0;
    size_t len = static_cast<size_t>(ret.value());
    if (out + len + 1 > buffsz)
      return 0;
    for (size_t j = 0; j < len; ++j)
      buffer[out++] = static_cast<wchar_t>(static_cast<unsigned char>(text[j]));
  }

  if (out + 1 > buffsz)
    return 0;
  buffer[out] = L'\0';
  return out;
}

} // namespace LIBC_NAMESPACE_DECL
