//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for arpa/nameser_compat.h
///
//===----------------------------------------------------------------------===//

#include "include/arpa/nameser_compat.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcArpaNameserCompatTest, OldNamesMatchTheNewOnes) {
  EXPECT_EQ(int(PACKETSZ), int(NS_PACKETSZ));
  EXPECT_EQ(int(HFIXEDSZ), int(NS_HFIXEDSZ));
  EXPECT_EQ(int(MAXDNAME), int(NS_MAXDNAME));
  EXPECT_EQ(int(T_A), int(ns_t_a));
  EXPECT_EQ(int(C_IN), int(ns_c_in));
  EXPECT_EQ(int(NOERROR), int(ns_r_noerror));
}
