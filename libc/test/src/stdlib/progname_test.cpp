//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the tests of getprogname and setprogname.
///
//===----------------------------------------------------------------------===//

#include "src/compat/progname.h"
#include "src/stdlib/getprogname.h"
#include "src/stdlib/setprogname.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcProgNameTest, SetKeepsTheLastComponent) {
  char *saved = LIBC_NAMESPACE::__progname;

  LIBC_NAMESPACE::setprogname("/usr/bin/tool");
  ASSERT_STREQ(LIBC_NAMESPACE::getprogname(), "tool");
  LIBC_NAMESPACE::setprogname("plain");
  ASSERT_STREQ(LIBC_NAMESPACE::getprogname(), "plain");
  LIBC_NAMESPACE::setprogname("dir/");
  ASSERT_STREQ(LIBC_NAMESPACE::getprogname(), "");

  LIBC_NAMESPACE::__progname = nullptr;
  ASSERT_STREQ(LIBC_NAMESPACE::getprogname(), "");
  LIBC_NAMESPACE::__progname = saved;
}
