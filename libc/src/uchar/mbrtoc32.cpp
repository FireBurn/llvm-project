//===-- Implementation of mbrtoc32 ----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/uchar/mbrtoc32.h"

#include "hdr/types/char32_t.h"
#include "hdr/types/mbstate_t.h"
#include "hdr/types/size_t.h"
#include "hdr/types/wchar_t.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/__support/wchar/mbrtowc.h"
#include "src/__support/wchar/mbstate.h"

namespace LIBC_NAMESPACE_DECL {

// The multibyte encoding here is UTF-8 and wchar_t holds a whole code point,
// so this is the same conversion mbrtowc makes, only into a char32_t.
LLVM_LIBC_FUNCTION(size_t, mbrtoc32,
                   (char32_t *__restrict pc32, const char *__restrict s,
                    size_t n, mbstate_t *__restrict ps)) {
  static internal::mbstate internal_mbstate;
  wchar_t wc;
  auto ret = internal::mbrtowc(&wc, s, n,
                               ps == nullptr
                                   ? &internal_mbstate
                                   : reinterpret_cast<internal::mbstate *>(ps));
  if (!ret.has_value()) {
    libc_errno = ret.error();
    return -1;
  }
  if (s != nullptr && pc32 != nullptr && ret.value() != static_cast<size_t>(-2))
    *pc32 = static_cast<char32_t>(wc);
  return ret.value();
}

} // namespace LIBC_NAMESPACE_DECL
