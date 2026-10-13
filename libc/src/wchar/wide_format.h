//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Formatting for the wide printf family
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_WCHAR_WIDE_FORMAT_H
#define LLVM_LIBC_SRC_WCHAR_WIDE_FORMAT_H

#include "hdr/types/wchar_t.h"
#include "src/__support/macros/config.h"

#include <stdarg.h>

namespace LIBC_NAMESPACE_DECL {
namespace internal {

// The format with each character taken down to the byte it stands for, in
// memory from malloc. Returns nullptr when a character has no such byte, or
// when there is no memory.
char *narrow_format(const wchar_t *format);

// The text as UTF-8, in memory from malloc, or nullptr with errno set.
char *encode_text(const wchar_t *text);

// Formats as printf would and hands back the result as wide characters, in
// memory from malloc that the caller frees. Returns how many there are, not
// counting the terminating null, or -1 with errno set.
int format_wide(const wchar_t *format, va_list vlist, wchar_t **out);

} // namespace internal
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_WCHAR_WIDE_FORMAT_H
