//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for strerrorname_np.
///
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "src/string/strerrorname_np.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcStrerrornameNpTest, NamesTheError) {
  ASSERT_STREQ(LIBC_NAMESPACE::strerrorname_np(EPERM), "EPERM");
  ASSERT_STREQ(LIBC_NAMESPACE::strerrorname_np(ENOENT), "ENOENT");
  ASSERT_STREQ(LIBC_NAMESPACE::strerrorname_np(EDOM), "EDOM");
  ASSERT_STREQ(LIBC_NAMESPACE::strerrorname_np(0), "0");
}

TEST(LlvmLibcStrerrornameNpTest, NoNameForAnUnknownError) {
  ASSERT_TRUE(LIBC_NAMESPACE::strerrorname_np(12345) == nullptr);
  ASSERT_TRUE(LIBC_NAMESPACE::strerrorname_np(-1) == nullptr);
}
