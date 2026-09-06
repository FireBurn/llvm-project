//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of getutline_r
///
//===----------------------------------------------------------------------===//

#include "src/utmp/getutline_r.h"

#include "hdr/types/struct_utmp.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/string/memory_utils/inline_memcpy.h"
#include "src/utmp/getutline.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, getutline_r,
                   (const struct utmp *line, struct utmp *buffer,
                    struct utmp **result)) {
  if (buffer == nullptr || result == nullptr)
    return -1;
  struct utmp *entry = internal::getutline_impl(line);
  if (entry == nullptr) {
    *result = nullptr;
    return -1;
  }
  inline_memcpy(buffer, entry, sizeof(*buffer));
  *result = buffer;
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL
