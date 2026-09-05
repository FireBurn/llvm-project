//===-- Unittests for the syslog functions --------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/syslog_macros.h"
#include "src/syslog/closelog.h"
#include "src/syslog/openlog.h"
#include "src/syslog/setlogmask.h"
#include "src/syslog/syslog.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcSyslogTest, PriorityMacros) {
  // A priority is a facility and a level in the one int.
  ASSERT_EQ(LOG_PRI(LOG_DAEMON | LOG_ERR), LOG_ERR);
  ASSERT_EQ(LOG_FAC(LOG_DAEMON | LOG_ERR), 3);
  ASSERT_EQ(LOG_MAKEPRI(LOG_DAEMON, LOG_ERR), LOG_DAEMON | LOG_ERR);
  ASSERT_EQ(LOG_MASK(LOG_ERR), 1 << LOG_ERR);
  ASSERT_EQ(LOG_UPTO(LOG_ERR), (1 << (LOG_ERR + 1)) - 1);
  ASSERT_EQ(LOG_UPTO(LOG_DEBUG), 0xff);
}

TEST(LlvmLibcSyslogTest, SetLogMask) {
  // The old mask comes back, and it starts out letting everything through.
  int first = LIBC_NAMESPACE::setlogmask(LOG_UPTO(LOG_WARNING));
  ASSERT_EQ(first, 0xff);

  int second = LIBC_NAMESPACE::setlogmask(LOG_MASK(LOG_ERR));
  ASSERT_EQ(second, LOG_UPTO(LOG_WARNING));

  // A mask of zero asks what the mask is without changing it.
  ASSERT_EQ(LIBC_NAMESPACE::setlogmask(0), LOG_MASK(LOG_ERR));
  ASSERT_EQ(LIBC_NAMESPACE::setlogmask(0), LOG_MASK(LOG_ERR));

  LIBC_NAMESPACE::setlogmask(0xff);
}

TEST(LlvmLibcSyslogTest, NoDaemonIsNotAnError) {
  // Nothing is listening in a test run, so these do nothing. They must
  // still return, rather than blocking or faulting.
  LIBC_NAMESPACE::openlog("libc_test", LOG_PID, LOG_USER);
  LIBC_NAMESPACE::syslog(LOG_ERR, "message with args %d %s", 1, "two");
  LIBC_NAMESPACE::syslog(LOG_INFO, "errno spelled out: %m");
  LIBC_NAMESPACE::closelog();

  // Closing when nothing was opened is allowed too.
  LIBC_NAMESPACE::closelog();
}

TEST(LlvmLibcSyslogTest, MaskedOutMessagesAreDropped) {
  LIBC_NAMESPACE::setlogmask(LOG_UPTO(LOG_ERR));
  LIBC_NAMESPACE::openlog("libc_test", 0, LOG_USER);
  LIBC_NAMESPACE::syslog(LOG_DEBUG, "dropped");
  LIBC_NAMESPACE::syslog(LOG_ERR, "kept");
  LIBC_NAMESPACE::closelog();
  LIBC_NAMESPACE::setlogmask(0xff);
}
