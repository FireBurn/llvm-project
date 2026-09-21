//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for wcsftime
///
//===----------------------------------------------------------------------===//

#include "hdr/types/struct_tm.h"
#include "hdr/types/wchar_t.h"
#include "src/wchar/wcsftime.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/Test.h"

using LlvmLibcWcsftimeTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;

static struct tm sample() {
  struct tm tm = {};
  tm.tm_year = 124; // 2024
  tm.tm_mon = 2;    // March
  tm.tm_mday = 9;
  tm.tm_hour = 7;
  tm.tm_min = 5;
  tm.tm_sec = 3;
  tm.tm_wday = 6; // Saturday
  tm.tm_yday = 68;
  return tm;
}

TEST_F(LlvmLibcWcsftimeTest, FormatsNumericFields) {
  struct tm tm = sample();
  wchar_t out[64];
  size_t n = LIBC_NAMESPACE::wcsftime(out, 64, L"%Y-%m-%d %H:%M:%S", &tm);
  EXPECT_EQ(n, size_t(19));
  EXPECT_EQ(out[0], L'2');
  EXPECT_EQ(out[4], L'-');
  EXPECT_EQ(out[5], L'0');
  EXPECT_EQ(out[6], L'3');
  EXPECT_EQ(out[18], L'3');
  EXPECT_EQ(out[19], L'\0');
}

TEST_F(LlvmLibcWcsftimeTest, FormatsNamesInTheCLocale) {
  struct tm tm = sample();
  wchar_t out[64];
  size_t n = LIBC_NAMESPACE::wcsftime(out, 64, L"%a %A %b %B", &tm);
  const wchar_t expected[] = L"Sat Saturday Mar March";
  ASSERT_EQ(n, sizeof(expected) / sizeof(wchar_t) - 1);
  for (size_t i = 0; i <= n; ++i)
    EXPECT_EQ(out[i], expected[i]);
}

TEST_F(LlvmLibcWcsftimeTest, KeepsWideLiteralText) {
  struct tm tm = sample();
  wchar_t out[16];
  size_t n = LIBC_NAMESPACE::wcsftime(out, 16, L"é %d 一", &tm);
  ASSERT_EQ(n, size_t(6));
  EXPECT_EQ(out[0], wchar_t(0xe9));
  EXPECT_EQ(out[2], L'0');
  EXPECT_EQ(out[3], L'9');
  EXPECT_EQ(out[5], wchar_t(0x4e00));
}

// Too small a buffer gives zero, and the contents are then undefined.
TEST_F(LlvmLibcWcsftimeTest, ReturnsZeroWhenTheResultDoesNotFit) {
  struct tm tm = sample();
  wchar_t out[4];
  EXPECT_EQ(LIBC_NAMESPACE::wcsftime(out, 4, L"%Y-%m-%d", &tm), size_t(0));
}

TEST_F(LlvmLibcWcsftimeTest, EmptyFormatWritesOnlyTheTerminator) {
  struct tm tm = sample();
  wchar_t out[4] = {L'x', L'x', L'x', L'x'};
  EXPECT_EQ(LIBC_NAMESPACE::wcsftime(out, 4, L"", &tm), size_t(0));
  EXPECT_EQ(out[0], L'\0');
}
