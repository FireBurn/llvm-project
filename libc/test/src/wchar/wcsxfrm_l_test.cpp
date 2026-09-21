//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for wcsxfrm_l and wcscoll_l
///
//===----------------------------------------------------------------------===//

#include "hdr/types/locale_t.h"
#include "src/wchar/wcscoll_l.h"
#include "src/wchar/wcsxfrm_l.h"
#include "test/UnitTest/Test.h"

// Collation here is by code point, so the transformed string is the string.
TEST(LlvmLibcWcsxfrmLocaleTest, TransformsAndComparesLikeTheOthers) {
  const locale_t loc = nullptr;
  wchar_t out[8] = {};
  ASSERT_EQ(LIBC_NAMESPACE::wcsxfrm_l(out, L"abc", 8, loc), size_t(3));
  EXPECT_EQ(out[1], L'b');
  // Nothing fits, so only the length is reported.
  EXPECT_EQ(LIBC_NAMESPACE::wcsxfrm_l(nullptr, L"abcdef", 0, loc), size_t(6));
  EXPECT_LT(LIBC_NAMESPACE::wcscoll_l(L"abc", L"abd", loc), 0);
  EXPECT_EQ(LIBC_NAMESPACE::wcscoll_l(L"abc", L"abc", loc), 0);
  EXPECT_GT(LIBC_NAMESPACE::wcscoll_l(L"b", L"a", loc), 0);
}
