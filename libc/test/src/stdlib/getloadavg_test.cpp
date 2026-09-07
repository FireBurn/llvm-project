//===-- Unittests for getloadavg ------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/stdlib/getloadavg.h"
#include "test/UnitTest/Test.h"

// The kernel keeps three averages, so asking for three fills three.
TEST(LlvmLibcGetLoadAvgTest, ReadsTheThreeAverages) {
  double averages[3] = {-1.0, -1.0, -1.0};
  ASSERT_EQ(LIBC_NAMESPACE::getloadavg(averages, 3), 3);
  for (double value : averages) {
    // A load average is a count of runnable tasks, so it is never negative.
    ASSERT_TRUE(value >= 0.0);
    ASSERT_TRUE(value < 100000.0);
  }
}

// Asking for fewer fills only that many and leaves the rest alone.
TEST(LlvmLibcGetLoadAvgTest, FillsOnlyWhatWasAskedFor) {
  double averages[3] = {-1.0, -1.0, -1.0};
  ASSERT_EQ(LIBC_NAMESPACE::getloadavg(averages, 1), 1);
  ASSERT_TRUE(averages[0] >= 0.0);
  ASSERT_TRUE(averages[1] == -1.0);
  ASSERT_TRUE(averages[2] == -1.0);
}

// Asking for more than there are gives all of them rather than failing.
TEST(LlvmLibcGetLoadAvgTest, MoreThanThereAreGivesThree) {
  double averages[8];
  for (double &value : averages)
    value = -1.0;
  ASSERT_EQ(LIBC_NAMESPACE::getloadavg(averages, 8), 3);
  ASSERT_TRUE(averages[2] >= 0.0);
  ASSERT_TRUE(averages[3] == -1.0);
}

// Asking for none is not an error, it is simply nothing to do.
TEST(LlvmLibcGetLoadAvgTest, NoneIsNotAnError) {
  double averages[3];
  ASSERT_EQ(LIBC_NAMESPACE::getloadavg(averages, 0), 0);
  ASSERT_EQ(LIBC_NAMESPACE::getloadavg(averages, -1), 0);
}
