//===-- Implementation of c32rtomb ----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/uchar/c32rtomb.h"

#include "hdr/types/char32_t.h"
#include "hdr/types/mbstate_t.h"
#include "hdr/types/size_t.h"
#include "hdr/types/wchar_t.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/__support/wchar/mbstate.h"
#include "src/__support/wchar/wcrtomb.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(size_t, c32rtomb,
                   (char *__restrict s, char32_t c32,
                    mbstate_t *__restrict ps)) {
  static internal::mbstate internal_mbstate;

  // A null buffer asks what a null character would take, which is one byte.
  char buf[sizeof(char32_t) / sizeof(char)];
  if (s == nullptr) {
    s = buf;
    c32 = U'\0';
  }

  auto result = internal::wcrtomb(
      s, static_cast<wchar_t>(c32),
      ps == nullptr ? &internal_mbstate
                    : reinterpret_cast<internal::mbstate *>(ps));
  if (!result.has_value()) {
    libc_errno = result.error();
    return -1;
  }
  return result.value();
}

} // namespace LIBC_NAMESPACE_DECL
