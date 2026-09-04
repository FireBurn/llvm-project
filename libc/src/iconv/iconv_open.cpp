//===-- Implementation of iconv_open --------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/iconv/iconv_open.h"

#include "hdr/errno_macros.h"
#include "hdr/func/malloc.h"
#include "hdr/types/iconv_t.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/iconv/conversion.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(iconv_t, iconv_open,
                   (const char *tocode, const char *fromcode)) {
  const auto *to = iconv_internal::find_charset(tocode);
  const auto *from = iconv_internal::find_charset(fromcode);
  // A set which is not known is the one error iconv_open reports.
  if (to == nullptr || from == nullptr) {
    libc_errno = EINVAL;
    return reinterpret_cast<iconv_t>(-1);
  }

  auto *conv = reinterpret_cast<iconv_internal::Conversion *>(
      ::malloc(sizeof(iconv_internal::Conversion)));
  if (conv == nullptr) {
    libc_errno = ENOMEM;
    return reinterpret_cast<iconv_t>(-1);
  }
  conv->from = from->encoding;
  conv->to = to->encoding;
  conv->from_table = from->table;
  conv->to_table = to->table;
  return reinterpret_cast<iconv_t>(conv);
}

} // namespace LIBC_NAMESPACE_DECL
