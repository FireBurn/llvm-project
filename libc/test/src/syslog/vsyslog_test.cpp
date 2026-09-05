//===-- Unittests for vsyslog ---------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/__support/CPP/string_view.h"
#include "src/syslog/closelog.h"
#include "src/syslog/openlog.h"
#include "src/syslog/vsyslog.h"
#include "test/UnitTest/Test.h"
#include "test/src/syslog/log_receiver.h"

#include <stdarg.h>
#include <syslog.h>

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

TEST(LlvmLibcVsyslogTest, WritesAMessageAtEveryPriority) {
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
TEST(LlvmLibcVsyslogTest, WritesAMessageWithoutOpeningFirst) {
  LogReceiver log("vsyslog_test.sock");
  log_through_wrapper(LOG_INFO, "%s", "no openlog first");

  char buf[4096];
  string_view record = log.next(buf, sizeof(buf));
  ASSERT_TRUE(record.starts_with("<14>"));
  ASSERT_TRUE(record.ends_with(" no openlog first"));
}
