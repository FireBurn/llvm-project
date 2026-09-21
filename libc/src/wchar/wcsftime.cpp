//===-- Implementation of wcsftime ----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/wchar/wcsftime.h"

#include "hdr/types/size_t.h"
#include "hdr/types/struct_tm.h"
#include "hdr/types/wchar_t.h"
#include "src/__support/CPP/new.h"
#include "src/__support/alloc-checker.h"
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

  // Only the C locale exists here, where every character of a conversion
  // specification is ASCII and one byte wide. A format carrying anything
  // else cannot be one this library understands.
  size_t format_len = 0;
  while (format[format_len] != L'\0')
    ++format_len;

  AllocChecker ac;
  char *narrow_format = new (ac) char[format_len + 1];
  if (!ac)
    return 0;
  for (size_t i = 0; i < format_len; ++i) {
    if (format[i] < 0 || format[i] > 127) {
      delete[] narrow_format;
      return 0;
    }
    narrow_format[i] = static_cast<char>(format[i]);
  }
  narrow_format[format_len] = '\0';

  // The result is counted in characters, and each one written is a byte, so
  // a byte buffer of the same count holds whatever will fit.
  char *narrow = new (ac) char[buffsz > 0 ? buffsz : 1];
  if (!ac) {
    delete[] narrow_format;
    return 0;
  }

  printf_core::Writer writer = printf_core::make_drop_overflow_writer(
      narrow, (buffsz > 0 ? buffsz - 1 : 0));
  auto ret = strftime_core::strftime_main(&writer, narrow_format, timeptr);
  if (buffsz > 0) {
    const printf_core::WriteBuffer<char> &wb = writer.get_write_buffer();
    wb.buff[wb.buff_cur] = '\0';
  }

  size_t written = 0;
  if (!ret.has_value() || ret.value() >= buffsz) {
    written = 0;
  } else {
    written = ret.value();
    for (size_t i = 0; i < written; ++i)
      buffer[i] = static_cast<wchar_t>(static_cast<unsigned char>(narrow[i]));
    buffer[written] = L'\0';
  }

  delete[] narrow;
  delete[] narrow_format;
  return written;
}

} // namespace LIBC_NAMESPACE_DECL
