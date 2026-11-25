//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for what syslog sends to the system logger.
///
//===----------------------------------------------------------------------===//

#include "hdr/syslog_macros.h"
#include "src/__support/CPP/string_view.h"
#include "src/__support/libc_errno.h"
#include "src/syslog/closelog.h"
#include "src/syslog/openlog.h"
#include "src/syslog/setlogmask.h"
#include "src/syslog/syslog.h"
#include "src/syslog/vsyslog.h"
#include "test/UnitTest/Test.h"
#include "test/src/syslog/linux/log_receiver.h"

#include <stdarg.h>

using LIBC_NAMESPACE::cpp::string_view;
using LIBC_NAMESPACE::testing::LogReceiver;

namespace {

// vsyslog takes the arguments already gathered, which is how a caller with
// its own variadic wrapper reaches it.
void log_through_wrapper(int priority, const char *format, ...) {
  va_list args;
  va_start(args, format);
  LIBC_NAMESPACE::vsyslog(priority, format, args);
  va_end(args);
}

} // anonymous namespace

TEST(LlvmLibcSyslogSocketTest, NoDaemonIsNotAnError) {
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

TEST(LlvmLibcSyslogSocketTest, FormatsTheRecord) {
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

TEST(LlvmLibcSyslogSocketTest, MaskedOutMessagesAreDropped) {
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

TEST(LlvmLibcSyslogSocketTest, WritesAMessageAtEveryPriority) {
  LogReceiver log("vsyslog_test.sock");
  LIBC_NAMESPACE::openlog("llvm-libc-test", 0, LOG_USER);

  // LOG_USER is 1 << 3, and the level is added to it.
  struct Case {
    int priority;
    const char *prefix;
  };
  constexpr Case CASES[] = {{LOG_EMERG, "<8>"},    {LOG_ALERT, "<9>"},
                            {LOG_CRIT, "<10>"},    {LOG_ERR, "<11>"},
                            {LOG_WARNING, "<12>"}, {LOG_NOTICE, "<13>"},
                            {LOG_INFO, "<14>"},    {LOG_DEBUG, "<15>"}};
  char buf[4096];
  for (const Case &c : CASES) {
    log_through_wrapper(c.priority, "a message with %s and %d", "a string", 42);
    string_view record = log.next(buf, sizeof(buf));
    ASSERT_TRUE(record.starts_with(c.prefix));
    ASSERT_TRUE(
        record.ends_with(" llvm-libc-test: a message with a string and 42"));
  }

  LIBC_NAMESPACE::closelog();
}

// A message logged before openlog opens the log itself, which is what a
// program that never calls openlog relies on.
TEST(LlvmLibcSyslogSocketTest, WritesAMessageWithoutOpeningFirst) {
  LogReceiver log("vsyslog_test.sock");
  log_through_wrapper(LOG_INFO, "%s", "no openlog first");

  char buf[4096];
  string_view record = log.next(buf, sizeof(buf));
  ASSERT_TRUE(record.starts_with("<14>"));
  ASSERT_TRUE(record.ends_with(" no openlog first"));
}
