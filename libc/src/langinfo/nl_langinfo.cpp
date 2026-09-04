//===-- Implementation of nl_langinfo -------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/langinfo/nl_langinfo.h"

#include "hdr/types/nl_item.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/langinfo/nl_langinfo_l.h"
#include "src/locale/locale.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(char *, nl_langinfo, (nl_item item)) {
  return LIBC_NAMESPACE::nl_langinfo_l(item, locale);
}

} // namespace LIBC_NAMESPACE_DECL
