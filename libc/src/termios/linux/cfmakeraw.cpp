//===-- Linux implementation of cfmakeraw ---------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/termios/cfmakeraw.h"

#include "hdr/termios_macros.h"
#include "hdr/types/struct_termios.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

// Puts the settings into raw mode: no translation of any kind on the way in
// or out, no line editing, no signals from control characters, and a read
// which returns as soon as one byte has arrived.
LLVM_LIBC_FUNCTION(void, cfmakeraw, (struct termios * t)) {
  if (t == nullptr)
    return;

  t->c_iflag &= static_cast<tcflag_t>(
      ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON));
  t->c_oflag &= static_cast<tcflag_t>(~OPOST);
  t->c_lflag &=
      static_cast<tcflag_t>(~(ECHO | ECHONL | ICANON | ISIG | IEXTEN));
  t->c_cflag &= static_cast<tcflag_t>(~(CSIZE | PARENB));
  t->c_cflag |= CS8;
  t->c_cc[VMIN] = 1;
  t->c_cc[VTIME] = 0;
}

} // namespace LIBC_NAMESPACE_DECL
