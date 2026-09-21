//===-- Implementation of wcsxfrm_l ---------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/wchar/wcsxfrm_l.h"

#include "hdr/types/locale_t.h"
#include "hdr/types/size_t.h"
#include "hdr/types/wchar_t.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/wchar/wcsxfrm_impl.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(size_t, wcsxfrm_l,
                   (wchar_t *__restrict dest, const wchar_t *__restrict src,
                    size_t n, locale_t)) {
  return internal::wcsxfrm_impl(dest, src, n);
}

} // namespace LIBC_NAMESPACE_DECL
