//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of getgrent_r
///
//===----------------------------------------------------------------------===//

#include "src/grp/getgrent_r.h"

#include "hdr/errno_macros.h"
#include "hdr/types/size_t.h"
#include "hdr/types/struct_group.h"
#include "src/__support/CPP/span.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/__support/macros/null_check.h"
#include "src/grp/grp_utils.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, getgrent_r,
                   (struct group * grbuf, char *buf, size_t buflen,
                    struct group **grbufp)) {
  LIBC_CRASH_ON_NULLPTR(grbuf);
  LIBC_CRASH_ON_NULLPTR(buf);
  LIBC_CRASH_ON_NULLPTR(grbufp);

  *grbufp = nullptr;

  const auto res = grp::read_next(grbuf, cpp::span<char>(buf, buflen));
  if (!res.has_value())
    return res.error();

  if (!res.value())
    return ENOENT;

  *grbufp = grbuf;
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL
