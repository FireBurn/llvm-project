//===-- Unittests for the syslog functions --------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/syslog_macros.h"
#include "src/__support/CPP/string_view.h"
#include "src/__support/libc_errno.h"
#include "src/syslog/closelog.h"
#include "src/syslog/openlog.h"
#include "src/syslog/setlogmask.h"
#include "src/syslog/syslog.h"
#include "test/UnitTest/Test.h"
#include "test/src/syslog/log_receiver.h"

using LIBC_NAMESPACE::cpp::string_view;
using LIBC_NAMESPACE::testing::LogReceiver;

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
  // Nothing is bound at the path, so no connection can be made and the
  // messages are dropped. The calls must still return rather than block or
  // fault.
  LogReceiver log("syslog_nobody.sock", /*listen=*/false);
  LIBC_NAMESPACE::openlog("libc_test", LOG_PID, LOG_USER);
  LIBC_NAMESPACE::syslog(LOG_ERR, "message with args %d %s", 1, "two");
  LIBC_NAMESPACE::syslog(LOG_INFO, "errno spelled out: %m");
  LIBC_NAMESPACE::closelog();

  // Closing when nothing was opened is allowed too.
  LIBC_NAMESPACE::closelog();
}

TEST(LlvmLibcSyslogTest, FormatsTheRecord) {
  LogReceiver log("syslog_test.sock");
  LIBC_NAMESPACE::openlog("libc_test", LOG_PID, LOG_DAEMON);
  char buf[4096];

  // The record opens with the facility and level as one number, and closes
  // with the tag, the process and the message.
  LIBC_NAMESPACE::syslog(LOG_ERR, "message with args %d %s", 1, "two");
  string_view record = log.next(buf, sizeof(buf));
  ASSERT_TRUE(record.starts_with("<27>"));
  ASSERT_TRUE(record.ends_with("]: message with args 1 two"));

  // A facility given with the priority is used instead of the one openlog
  // set.
  LIBC_NAMESPACE::syslog(LOG_USER | LOG_INFO, "facility %s", "given");
  record = log.next(buf, sizeof(buf));
  ASSERT_TRUE(record.starts_with("<14>"));

  LIBC_NAMESPACE::libc_errno = ENOENT;
  LIBC_NAMESPACE::syslog(LOG_INFO, "errno spelled out: %m");
  LIBC_NAMESPACE::libc_errno = 0;
  record = log.next(buf, sizeof(buf));
  ASSERT_TRUE(record.ends_with("errno spelled out: No such file or directory"));

  LIBC_NAMESPACE::closelog();
}

TEST(LlvmLibcSyslogTest, MaskedOutMessagesAreDropped) {
  LogReceiver log("syslog_test.sock");
  LIBC_NAMESPACE::setlogmask(LOG_UPTO(LOG_ERR));
  LIBC_NAMESPACE::openlog("libc_test", 0, LOG_USER);
  LIBC_NAMESPACE::syslog(LOG_DEBUG, "dropped");
  LIBC_NAMESPACE::syslog(LOG_ERR, "kept");

  char buf[4096];
  ASSERT_TRUE(log.next(buf, sizeof(buf)).ends_with("libc_test: kept"));
  ASSERT_EQ(log.next(buf, sizeof(buf)).size(), size_t(0));

  LIBC_NAMESPACE::closelog();
  LIBC_NAMESPACE::setlogmask(0xff);
}
