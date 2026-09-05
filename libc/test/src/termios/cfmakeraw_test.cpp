//===-- Unittests for cfmakeraw -------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/termios_macros.h"
#include "hdr/types/struct_termios.h"
#include "src/termios/cfmakeraw.h"
#include "test/UnitTest/Test.h"

// Raw mode is the terminal passing bytes through untouched: no line editing,
// no echo, no signals from control characters, and no translation either
// way. cfmakeraw turns all of that off in the settings it is given.
TEST(LlvmLibcCfmakerawTest, TurnsOffEverythingThatChangesTheBytes) {
  struct termios settings = {};
  settings.c_iflag = ~tcflag_t(0);
  settings.c_oflag = ~tcflag_t(0);
  settings.c_lflag = ~tcflag_t(0);
  settings.c_cflag = ~tcflag_t(0);

  LIBC_NAMESPACE::cfmakeraw(&settings);

  ASSERT_EQ(settings.c_iflag & tcflag_t(IGNBRK | BRKINT | PARMRK | ISTRIP |
                                        INLCR | IGNCR | ICRNL | IXON),
            tcflag_t(0));
  ASSERT_EQ(settings.c_oflag & tcflag_t(OPOST), tcflag_t(0));
  ASSERT_EQ(settings.c_lflag & tcflag_t(ECHO | ECHONL | ICANON | ISIG | IEXTEN),
            tcflag_t(0));
  // The character size is set to eight bits, and CS8 is itself one of the
  // values CSIZE covers, so what is left of that field is CS8 rather than
  // nothing. Parity is off.
  ASSERT_EQ(settings.c_cflag & tcflag_t(CSIZE), tcflag_t(CS8));
  ASSERT_EQ(settings.c_cflag & tcflag_t(PARENB), tcflag_t(0));

  // A read returns as soon as there is one byte, and waits forever for it.
  ASSERT_EQ(int(settings.c_cc[VMIN]), 1);
  ASSERT_EQ(int(settings.c_cc[VTIME]), 0);
}
