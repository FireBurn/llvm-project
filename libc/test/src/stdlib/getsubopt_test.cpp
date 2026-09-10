//===-- Unittests for getsubopt -------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/stdlib/getsubopt.h"
#include "test/UnitTest/Test.h"

namespace {

char *const TOKENS[] = {const_cast<char *>("ro"), const_cast<char *>("rw"),
                        const_cast<char *>("size"), nullptr};

} // anonymous namespace

TEST(LlvmLibcGetsuboptTest, ReadsEachSuboptionInTurn) {
  char options[] = "ro,rw";
  char *cursor = options;
  char *value = reinterpret_cast<char *>(1);

  ASSERT_EQ(LIBC_NAMESPACE::getsubopt(&cursor, TOKENS, &value), 0);
  ASSERT_TRUE(value == nullptr);
  ASSERT_EQ(LIBC_NAMESPACE::getsubopt(&cursor, TOKENS, &value), 1);
  ASSERT_TRUE(value == nullptr);
  // Nothing is left, which is reported the same way an unknown one is, so
  // the empty string is what tells the caller to stop.
  ASSERT_EQ(LIBC_NAMESPACE::getsubopt(&cursor, TOKENS, &value), -1);
  ASSERT_EQ(*cursor, '\0');
}

TEST(LlvmLibcGetsuboptTest, HandsBackTheValueAfterTheEquals) {
  char options[] = "size=10,ro";
  char *cursor = options;
  char *value = nullptr;

  ASSERT_EQ(LIBC_NAMESPACE::getsubopt(&cursor, TOKENS, &value), 2);
  ASSERT_FALSE(value == nullptr);
  ASSERT_STREQ(value, "10");

  ASSERT_EQ(LIBC_NAMESPACE::getsubopt(&cursor, TOKENS, &value), 0);
  ASSERT_TRUE(value == nullptr);
}

TEST(LlvmLibcGetsuboptTest, AnUnknownSuboptionIsHandedBackWhole) {
  char options[] = "nosuch,ro";
  char *cursor = options;
  char *value = nullptr;

  ASSERT_EQ(LIBC_NAMESPACE::getsubopt(&cursor, TOKENS, &value), -1);
  ASSERT_FALSE(value == nullptr);
  ASSERT_STREQ(value, "nosuch");
  // The scan carries on past it, so one name the caller does not know does
  // not cost it the rest of the list.
  ASSERT_EQ(LIBC_NAMESPACE::getsubopt(&cursor, TOKENS, &value), 0);
}

TEST(LlvmLibcGetsuboptTest, AnUnknownSuboptionKeepsItsValue) {
  char options[] = "nosuch=7";
  char *cursor = options;
  char *value = nullptr;

  ASSERT_EQ(LIBC_NAMESPACE::getsubopt(&cursor, TOKENS, &value), -1);
  ASSERT_STREQ(value, "nosuch=7");
}

TEST(LlvmLibcGetsuboptTest, NothingToRead) {
  char options[] = "";
  char *cursor = options;
  char *value = reinterpret_cast<char *>(1);

  ASSERT_EQ(LIBC_NAMESPACE::getsubopt(&cursor, TOKENS, &value), -1);
  ASSERT_TRUE(value == nullptr);
}
