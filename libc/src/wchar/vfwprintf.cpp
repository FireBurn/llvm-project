//===-- Implementation of vfwprintf ---------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/wchar/vfwprintf.h"

#include "hdr/func/free.h"
#include "hdr/types/size_t.h"
#include "src/__support/File/file.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/wchar/wide_format.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, vfwprintf,
                   (::FILE *__restrict stream, const wchar_t *__restrict format,
                    va_list vlist)) {
  wchar_t *wide = nullptr;
  int count = internal::format_wide(format, vlist, &wide);
  if (count < 0)
    return -1;

  // The stream's wide path takes the orientation, and refuses a stream
  // already given to bytes.
  auto *file = reinterpret_cast<File *>(stream);
  FileIOResult result = file->write(wide, static_cast<size_t>(count));
  free(wide);
  if (result.has_error() || result.value < static_cast<size_t>(count)) {
    if (result.has_error())
      libc_errno = result.error;
    return -1;
  }
  return count;
}

} // namespace LIBC_NAMESPACE_DECL
