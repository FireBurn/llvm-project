//===-- Linux implementation of cfsetspeed --------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/termios/cfsetspeed.h"

#include "hdr/types/speed_t.h"
#include "hdr/types/struct_termios.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/__support/macros/null_check.h"
#include "src/termios/cfsetispeed.h"
#include "src/termios/cfsetospeed.h"

namespace LIBC_NAMESPACE_DECL {

// Sets both speeds at once, which is what a caller wants nearly every time
// and is why BSD added it. Either failing leaves the other set, the same as
// setting the two separately would.
LLVM_LIBC_FUNCTION(int, cfsetspeed, (struct termios * t, speed_t speed)) {
  LIBC_CRASH_ON_NULLPTR(t);
  if (LIBC_NAMESPACE::cfsetospeed(t, speed) != 0)
    return -1;
  return LIBC_NAMESPACE::cfsetispeed(t, speed);
}

} // namespace LIBC_NAMESPACE_DECL
