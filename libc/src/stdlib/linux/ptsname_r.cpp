//===-- Linux implementation of ptsname_r ---------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/stdlib/ptsname_r.h"

#include "hdr/errno_macros.h"
#include "hdr/types/size_t.h"
#include "src/__support/common.h"
#include "src/__support/integer_to_string.h"
#include "src/__support/macros/config.h"
#include "src/stdlib/linux/pty_utils.h"

namespace LIBC_NAMESPACE_DECL {

// Returns the error rather than setting errno, which is what the reentrant
// form of this is expected to do.
LLVM_LIBC_FUNCTION(int, ptsname_r, (int fd, char *buf, size_t buflen)) {
  if (buf == nullptr)
    return EINVAL;

  unsigned int number = 0;
  if (int err = pty::number(fd, &number); err != 0)
    return err;

  const IntegerToString<unsigned int> digits(number);
  auto view = digits.view();
  if (pty::PTS_PREFIX_LEN + view.size() + 1 > buflen)
    return ERANGE;

  for (size_t i = 0; i < pty::PTS_PREFIX_LEN; ++i)
    buf[i] = pty::PTS_PREFIX[i];
  for (size_t i = 0; i < view.size(); ++i)
    buf[pty::PTS_PREFIX_LEN + i] = view[i];
  buf[pty::PTS_PREFIX_LEN + view.size()] = '\0';
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL
