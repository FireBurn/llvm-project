//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the syslog name tables
///
//===----------------------------------------------------------------------===//

#define SYSLOG_NAMES 1
#include "include/syslog.h"
#include "test/UnitTest/Test.h"

static bool same(const char *a, const char *b) {
  while (*a != '\0' && *a == *b) {
    ++a;
    ++b;
  }
  return *a == *b;
}

static int find(const CODE *table, const char *name) {
  for (; table->c_name != nullptr; ++table)
    if (same(table->c_name, name))
      return table->c_val;
  return -2;
}

TEST(LlvmLibcSyslogNameTablesTest, PrioritiesAreFoundByName) {
  EXPECT_EQ(find(prioritynames, "emerg"), int(LOG_EMERG));
  EXPECT_EQ(find(prioritynames, "warning"), int(LOG_WARNING));
  EXPECT_EQ(find(prioritynames, "warn"), int(LOG_WARNING));
  EXPECT_EQ(find(prioritynames, "none"), int(INTERNAL_NOPRI));
  EXPECT_EQ(find(prioritynames, "nonesuch"), -2);
}

TEST(LlvmLibcSyslogNameTablesTest, FacilitiesAreFoundByName) {
  EXPECT_EQ(find(facilitynames, "daemon"), int(LOG_DAEMON));
  EXPECT_EQ(find(facilitynames, "kern"), int(LOG_KERN));
  EXPECT_EQ(find(facilitynames, "local7"), int(LOG_LOCAL7));
}
