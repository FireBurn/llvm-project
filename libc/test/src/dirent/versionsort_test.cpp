//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for versionsort
///
//===----------------------------------------------------------------------===//

#include "hdr/types/struct_dirent.h"
#include "src/dirent/versionsort.h"
#include "src/string/strcpy.h"
#include "test/UnitTest/Test.h"

static int compare(const char *a, const char *b) {
  struct dirent first = {};
  struct dirent second = {};
  LIBC_NAMESPACE::strcpy(first.d_name, a);
  LIBC_NAMESPACE::strcpy(second.d_name, b);
  const struct dirent *pa = &first;
  const struct dirent *pb = &second;
  return LIBC_NAMESPACE::versionsort(&pa, &pb);
}

TEST(LlvmLibcVersionsortTest, DigitsAreReadAsNumbers) {
  ASSERT_LT(compare("file9", "file10"), 0);
  ASSERT_GT(compare("file10", "file9"), 0);
  ASSERT_EQ(compare("file1", "file1"), 0);
}
