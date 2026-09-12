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
#include "src/locale/locale.h"

namespace LIBC_NAMESPACE_DECL {

namespace {

// An empty name, or libiconv's "CHAR", is the character set of the locale in
// force when the conversion is opened.
const iconv_internal::Charset *find(const char *name) {
  if (name != nullptr && (iconv_internal::same_name(name, "") ||
                          iconv_internal::same_name(name, "CHAR")))
    return iconv_internal::find_charset(internal::current_codeset());
  return iconv_internal::find_charset(name);
}

} // namespace

LLVM_LIBC_FUNCTION(iconv_t, iconv_open,
                   (const char *tocode, const char *fromcode)) {
  const auto *to = find(tocode);
  const auto *from = find(fromcode);
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
  conv->ignore = iconv_internal::has_flag(tocode, "IGNORE");
  // Until a byte order mark says otherwise, input is in the host's order.
  conv->read_mark = true;
  conv->read_big = !Endian::IS_LITTLE;
  conv->write_mark = true;
  return reinterpret_cast<iconv_t>(conv);
}

} // namespace LIBC_NAMESPACE_DECL
