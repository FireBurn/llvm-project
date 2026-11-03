//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of getspent_r
///
//===----------------------------------------------------------------------===//

#include "src/shadow/getspent_r.h"

#include "hdr/errno_macros.h"
#include "hdr/types/struct_spwd.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/shadow/shadow_utils.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, getspent_r,
                   (struct spwd * spbuf, char *buf, size_t buflen,
                    struct spwd **spbufp)) {
  if (spbufp != nullptr)
    *spbufp = nullptr;
  if (spbuf == nullptr || buf == nullptr || spbufp == nullptr)
    return EINVAL;
  auto entry = shadow_db::read_next(spbuf, cpp::span<char>(buf, buflen));
  if (!entry.has_value())
    return entry.error();
  if (!entry.value())
    return ENOENT;
  *spbufp = spbuf;
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL
