//===-- The POSIX strerror_r ------------------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC___SUPPORT_STRINGUTIL_XSI_STRERROR_R_H
#define LLVM_LIBC_SRC___SUPPORT_STRINGUTIL_XSI_STRERROR_R_H

#include "hdr/errno_macros.h"
#include "hdr/types/size_t.h"
#include "src/__support/StringUtil/error_to_string.h"
#include "src/__support/macros/config.h"
#include "src/string/memory_utils/inline_memcpy.h"

namespace LIBC_NAMESPACE_DECL {

// The strerror_r POSIX states: it fills the buffer and answers with an error
// number. Whether the number names an error at all is reported separately from
// whether the message fitted. A number that names none still gets a message,
// so it is the error table that says which it is.
LIBC_INLINE int xsi_strerror_r(int err_num, char *buf, size_t buflen) {
  if (buf == nullptr)
    return EINVAL;

  const cpp::string_view message = get_error_string(err_num);
  const bool known = try_get_errno_name(err_num).has_value();
  if (message.size() + 1 > buflen) {
    if (buflen > 0) {
      inline_memcpy(buf, message.data(), buflen - 1);
      buf[buflen - 1] = '\0';
    }
    return ERANGE;
  }
  inline_memcpy(buf, message.data(), message.size());
  buf[message.size()] = '\0';
  return known ? 0 : EINVAL;
}

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC___SUPPORT_STRINGUTIL_XSI_STRERROR_R_H
