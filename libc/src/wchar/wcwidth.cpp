//===-- Implementation of wcwidth -----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/wchar/wcwidth.h"

#include "hdr/stdint_proxy.h"
#include "hdr/types/wchar_t.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/wchar/wcwidth_table.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, wcwidth, (wchar_t wc)) {
  uint32_t cp = static_cast<uint32_t>(wc);
  if (cp > 0x10FFFF)
    return -1;
  if (wcwidth_internal::in_table(wcwidth_internal::NOT_PRINTABLE, cp))
    return -1;
  if (wcwidth_internal::in_table(wcwidth_internal::ZERO_WIDTH, cp))
    return 0;
  if (wcwidth_internal::in_table(wcwidth_internal::WIDE, cp))
    return 2;
  return 1;
}

} // namespace LIBC_NAMESPACE_DECL
