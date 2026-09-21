//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the _l forms of the wide character classification calls
///
//===----------------------------------------------------------------------===//

#include "hdr/types/locale_t.h"
#include "src/wctype/iswalnum_l.h"
#include "src/wctype/iswalpha_l.h"
#include "src/wctype/iswblank_l.h"
#include "src/wctype/iswcntrl_l.h"
#include "src/wctype/iswctype_l.h"
#include "src/wctype/iswdigit_l.h"
#include "src/wctype/iswgraph_l.h"
#include "src/wctype/iswlower_l.h"
#include "src/wctype/iswprint_l.h"
#include "src/wctype/iswpunct_l.h"
#include "src/wctype/iswspace_l.h"
#include "src/wctype/iswupper_l.h"
#include "src/wctype/iswxdigit_l.h"
#include "src/wctype/towlower_l.h"
#include "src/wctype/towupper_l.h"
#include "src/wctype/wctype_l.h"
#include "test/UnitTest/Test.h"

// The classes are the same as the ones without a locale, which have tests of
// their own, so these check that each name reaches the right class.
TEST(LlvmLibcWctypeLocaleTest, EachNameAsksTheRightQuestion) {
  const locale_t loc = nullptr;
  EXPECT_NE(LIBC_NAMESPACE::iswalnum_l(L'a', loc), 0);
  EXPECT_EQ(LIBC_NAMESPACE::iswalnum_l(L'-', loc), 0);
  EXPECT_NE(LIBC_NAMESPACE::iswalpha_l(L'a', loc), 0);
  EXPECT_EQ(LIBC_NAMESPACE::iswalpha_l(L'1', loc), 0);
  EXPECT_NE(LIBC_NAMESPACE::iswblank_l(L'\t', loc), 0);
  EXPECT_EQ(LIBC_NAMESPACE::iswblank_l(L'\n', loc), 0);
  EXPECT_NE(LIBC_NAMESPACE::iswcntrl_l(L'\n', loc), 0);
  EXPECT_EQ(LIBC_NAMESPACE::iswcntrl_l(L'a', loc), 0);
  EXPECT_NE(LIBC_NAMESPACE::iswdigit_l(L'7', loc), 0);
  EXPECT_EQ(LIBC_NAMESPACE::iswdigit_l(L'a', loc), 0);
  EXPECT_NE(LIBC_NAMESPACE::iswgraph_l(L'!', loc), 0);
  EXPECT_EQ(LIBC_NAMESPACE::iswgraph_l(L' ', loc), 0);
  EXPECT_NE(LIBC_NAMESPACE::iswlower_l(L'a', loc), 0);
  EXPECT_EQ(LIBC_NAMESPACE::iswlower_l(L'A', loc), 0);
  EXPECT_NE(LIBC_NAMESPACE::iswprint_l(L' ', loc), 0);
  EXPECT_EQ(LIBC_NAMESPACE::iswprint_l(L'\x01', loc), 0);
  EXPECT_NE(LIBC_NAMESPACE::iswpunct_l(L'!', loc), 0);
  EXPECT_EQ(LIBC_NAMESPACE::iswpunct_l(L'a', loc), 0);
  EXPECT_NE(LIBC_NAMESPACE::iswspace_l(L' ', loc), 0);
  EXPECT_EQ(LIBC_NAMESPACE::iswspace_l(L'a', loc), 0);
  EXPECT_NE(LIBC_NAMESPACE::iswupper_l(L'A', loc), 0);
  EXPECT_EQ(LIBC_NAMESPACE::iswupper_l(L'a', loc), 0);
  EXPECT_NE(LIBC_NAMESPACE::iswxdigit_l(L'f', loc), 0);
  EXPECT_EQ(LIBC_NAMESPACE::iswxdigit_l(L'g', loc), 0);
}

TEST(LlvmLibcWctypeLocaleTest, CaseMapping) {
  const locale_t loc = nullptr;
  EXPECT_EQ(LIBC_NAMESPACE::towlower_l(L'A', loc), wint_t(L'a'));
  EXPECT_EQ(LIBC_NAMESPACE::towlower_l(L'a', loc), wint_t(L'a'));
  EXPECT_EQ(LIBC_NAMESPACE::towlower_l(L'1', loc), wint_t(L'1'));
  EXPECT_EQ(LIBC_NAMESPACE::towupper_l(L'a', loc), wint_t(L'A'));
  EXPECT_EQ(LIBC_NAMESPACE::towupper_l(L'A', loc), wint_t(L'A'));
}

TEST(LlvmLibcWctypeLocaleTest, ClassesByName) {
  const locale_t loc = nullptr;
  wctype_t digit = LIBC_NAMESPACE::wctype_l("digit", loc);
  ASSERT_NE(digit, wctype_t(0));
  EXPECT_NE(LIBC_NAMESPACE::iswctype_l(L'5', digit, loc), 0);
  EXPECT_EQ(LIBC_NAMESPACE::iswctype_l(L'x', digit, loc), 0);
  EXPECT_EQ(LIBC_NAMESPACE::wctype_l("no such class", loc), wctype_t(0));
}
