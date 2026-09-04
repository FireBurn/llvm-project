//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of confstr
///
//===----------------------------------------------------------------------===//

#include "src/unistd/confstr.h"

#include "hdr/errno_macros.h"
#include "hdr/types/size_t.h"
#include "hdr/unistd_macros.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(size_t, confstr, (int name, char *buf, size_t len)) {
  // Only _CS_PATH is defined. The value is the directories in which the
  // standard utilities are guaranteed to be found, which is what a caller
  // asks for when it must not trust the inherited PATH.
  if (name != _CS_PATH) {
    libc_errno = EINVAL;
    return 0;
  }

  static constexpr char PATH_VALUE[] = "/bin:/usr/bin";
  constexpr size_t NEEDED = sizeof(PATH_VALUE);

  // A length of zero is how a caller asks how much room it needs, so the
  // buffer is left alone and only the size is reported.
  if (buf != nullptr && len != 0) {
    size_t to_copy = len < NEEDED ? len : NEEDED;
    for (size_t i = 0; i + 1 < to_copy; ++i)
      buf[i] = PATH_VALUE[i];
    buf[to_copy - 1] = '\0';
  }
  return NEEDED;
}

} // namespace LIBC_NAMESPACE_DECL
