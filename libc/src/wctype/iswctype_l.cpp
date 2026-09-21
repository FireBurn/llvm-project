//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of iswctype_l
///
//===----------------------------------------------------------------------===//

#include "src/wctype/iswctype_l.h"

#include "hdr/types/locale_t.h"
#include "hdr/types/wctype_t.h"
#include "hdr/types/wint_t.h"
#include "src/__support/common.h"
#include "src/__support/wctype_impl.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, iswctype_l, (wint_t c, wctype_t desc, locale_t)) {
  return internal::iswctype(static_cast<wchar_t>(c), desc);
}

} // namespace LIBC_NAMESPACE_DECL
