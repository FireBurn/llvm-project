//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for lastlog.h bringing in utmp.h
///
//===----------------------------------------------------------------------===//

#include "include/lastlog.h"
#include "test/UnitTest/Test.h"

#ifndef _LLVM_LIBC_UTMP_H
#error "<lastlog.h> does not bring in <utmp.h>"
#endif

TEST(LlvmLibcLastlogUtmpTest, CarriesTheHeaders) {
  struct lastlog record;
  EXPECT_GT(sizeof(record), size_t(0));
}
