//===-- Unittests for vsyslog ---------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/syslog/closelog.h"
#include "src/syslog/openlog.h"
#include "src/syslog/vsyslog.h"
#include "test/UnitTest/Test.h"

#include <stdarg.h>
#include <syslog.h>

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

// Where there is no log to write to the message is dropped rather than
// reported, so what a test can say is that every priority and every
// conversion is accepted and nothing is reached for that is not there.
TEST(LlvmLibcVsyslogTest, WritesAMessageAtEveryPriority) {
  LIBC_NAMESPACE::openlog("llvm-libc-test", LOG_PID, LOG_USER);

  constexpr int PRIORITIES[] = {LOG_EMERG,   LOG_ALERT,  LOG_CRIT, LOG_ERR,
                                LOG_WARNING, LOG_NOTICE, LOG_INFO, LOG_DEBUG};
  for (int priority : PRIORITIES)
    log_through_wrapper(priority, "a message with %s and %d", "a string", 42);

  LIBC_NAMESPACE::closelog();
}

// A message logged before openlog opens the log itself, which is what a
// program that never calls openlog relies on.
TEST(LlvmLibcVsyslogTest, WritesAMessageWithoutOpeningFirst) {
  log_through_wrapper(LOG_INFO, "%s", "no openlog first");
  LIBC_NAMESPACE::closelog();
}
