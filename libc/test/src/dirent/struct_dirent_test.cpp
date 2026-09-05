//===-- Unittests for struct dirent ---------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/limits_macros.h"
#include "hdr/types/struct_dirent.h"
#include "src/string/strlen.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcStructDirentTest, NameHoldsAWholeName) {
  // A program is allowed to declare one of these itself and put a name in it,
  // so the array has to be as long as a name may be.
  static const struct dirent entry = {};
  EXPECT_EQ(sizeof(entry.d_name), size_t(NAME_MAX + 1));
}

TEST(LlvmLibcStructDirentTest, ANameCanBeWrittenIntoIt) {
  struct dirent entry = {};
  const char name[] = "a_name_longer_than_one_character";
  for (size_t i = 0; i < sizeof(name); ++i)
    entry.d_name[i] = name[i];
  EXPECT_EQ(LIBC_NAMESPACE::strlen(entry.d_name), sizeof(name) - 1);
}
