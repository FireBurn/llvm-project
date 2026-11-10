//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of ns_msg_getflag
///
//===----------------------------------------------------------------------===//

#include "src/arpa/nameser/ns_msg_getflag.h"

#include "hdr/errno_macros.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, ns_msg_getflag, (ns_msg handle, int flag)) {
  struct Field {
    unsigned mask;
    int shift;
  };
  // Where each ns_f_ flag is in the header's flags word.
  static constexpr Field FIELDS[] = {
      {0x8000, 15}, {0x7800, 11}, {0x0400, 10}, {0x0200, 9}, {0x0100, 8},
      {0x0080, 7},  {0x0040, 6},  {0x0020, 5},  {0x0010, 4}, {0x000f, 0}};
  if (flag < 0 || flag >= static_cast<int>(sizeof(FIELDS) / sizeof(Field))) {
    libc_errno = EINVAL;
    return -1;
  }
  return (handle._flags & FIELDS[flag].mask) >> FIELDS[flag].shift;
}

} // namespace LIBC_NAMESPACE_DECL
