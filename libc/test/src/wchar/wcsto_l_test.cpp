//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the wcsto*_l functions
///
//===----------------------------------------------------------------------===//

#include "src/wchar/wcstod_l.h"
#include "src/wchar/wcstof_l.h"
#include "src/wchar/wcstol_l.h"
#include "src/wchar/wcstold_l.h"
#include "src/wchar/wcstoll_l.h"
#include "src/wchar/wcstoul_l.h"
#include "src/wchar/wcstoull_l.h"

#include "test/UnitTest/Test.h"

TEST(LlvmLibcWcstoLTest, Integers) {
  wchar_t *end = nullptr;
  const wchar_t *str = L"  -1234xyz";
  ASSERT_EQ(LIBC_NAMESPACE::wcstol_l(str, &end, 10, nullptr), -1234L);
  ASSERT_TRUE(end == str + 7);
  ASSERT_EQ(LIBC_NAMESPACE::wcstoll_l(L"-0x10", &end, 16, nullptr), -16LL);
  ASSERT_EQ(LIBC_NAMESPACE::wcstoul_l(L"777", &end, 8, nullptr), 511UL);
  ASSERT_EQ(LIBC_NAMESPACE::wcstoull_l(L"42", nullptr, 0, nullptr), 42ULL);
}

TEST(LlvmLibcWcstoLTest, Floats) {
  wchar_t *end = nullptr;
  const wchar_t *str = L"2.5e1rest";
  ASSERT_TRUE(LIBC_NAMESPACE::wcstod_l(str, &end, nullptr) == 25.0);
  ASSERT_TRUE(end == str + 5);
  ASSERT_TRUE(LIBC_NAMESPACE::wcstof_l(L"-0.5", &end, nullptr) == -0.5f);
  ASSERT_TRUE(LIBC_NAMESPACE::wcstold_l(L"1.25", &end, nullptr) == 1.25L);
}
