//===-- Unittests for pause -----------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/signal_macros.h"
#include "src/signal/signal.h"
#include "src/unistd/alarm.h"
#include "src/unistd/pause.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

using LlvmLibcPauseTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Fails;

static volatile int alarm_count = 0;

static void handler(int) { ++alarm_count; }

// pause waits for a signal that is handled, and there is no other way for it
// to return: it reports EINTR having run the handler.
TEST_F(LlvmLibcPauseTest, ReturnsWhenASignalIsHandled) {
  ASSERT_NE(LIBC_NAMESPACE::signal(SIGALRM, &handler), SIG_ERR);
  alarm_count = 0;

  ASSERT_EQ(LIBC_NAMESPACE::alarm(1), 0U);
  ASSERT_THAT(LIBC_NAMESPACE::pause(), Fails(EINTR));
  ASSERT_EQ(alarm_count, 1);
}
