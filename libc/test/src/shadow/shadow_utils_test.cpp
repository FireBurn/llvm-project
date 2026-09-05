//===-- Unittests for the shadow line parser ------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/types/struct_spwd.h"
#include "src/__support/CPP/span.h"
#include "src/shadow/shadow_utils.h"
#include "test/UnitTest/Test.h"

using LIBC_NAMESPACE::cpp::span;

namespace {

bool parse(char *line, struct spwd *entry) {
  size_t len = 0;
  while (line[len] != '\0')
    ++len;
  return LIBC_NAMESPACE::internal::parse_line(span<char>(line, len + 1), entry);
}

} // anonymous namespace

TEST(LlvmLibcShadowTest, EveryFieldSet) {
  char line[] = "alice:$y$hash:19500:1:180:14:30:20000:0";
  struct spwd entry;
  ASSERT_TRUE(parse(line, &entry));
  ASSERT_STREQ(entry.sp_namp, "alice");
  ASSERT_STREQ(entry.sp_pwdp, "$y$hash");
  ASSERT_EQ(entry.sp_lstchg, 19500L);
  ASSERT_EQ(entry.sp_min, 1L);
  ASSERT_EQ(entry.sp_max, 180L);
  ASSERT_EQ(entry.sp_warn, 14L);
  ASSERT_EQ(entry.sp_inact, 30L);
  ASSERT_EQ(entry.sp_expire, 20000L);
  ASSERT_EQ(entry.sp_flag, 0UL);
}

TEST(LlvmLibcShadowTest, EmptyFieldsReadAsMinusOne) {
  // Everything past the hash may be left empty, and that is not zero: it
  // means there is no such limit.
  char line[] = "root:$6$hash:19000:0:99999:7:::";
  struct spwd entry;
  ASSERT_TRUE(parse(line, &entry));
  ASSERT_STREQ(entry.sp_namp, "root");
  ASSERT_EQ(entry.sp_lstchg, 19000L);
  ASSERT_EQ(entry.sp_min, 0L);
  ASSERT_EQ(entry.sp_max, 99999L);
  ASSERT_EQ(entry.sp_warn, 7L);
  ASSERT_EQ(entry.sp_inact, -1L);
  ASSERT_EQ(entry.sp_expire, -1L);
  ASSERT_EQ(entry.sp_flag, static_cast<unsigned long>(-1L));
}

TEST(LlvmLibcShadowTest, AllCountsEmpty) {
  char line[] = "bob:!:::::::";
  struct spwd entry;
  ASSERT_TRUE(parse(line, &entry));
  ASSERT_STREQ(entry.sp_namp, "bob");
  ASSERT_STREQ(entry.sp_pwdp, "!");
  ASSERT_EQ(entry.sp_lstchg, -1L);
  ASSERT_EQ(entry.sp_min, -1L);
  ASSERT_EQ(entry.sp_max, -1L);
  ASSERT_EQ(entry.sp_warn, -1L);
  ASSERT_EQ(entry.sp_inact, -1L);
  ASSERT_EQ(entry.sp_expire, -1L);
}

TEST(LlvmLibcShadowTest, LockedAccountKeepsItsMarker) {
  // A leading exclamation mark is how an account is locked, and it is part
  // of the field rather than something to strip.
  char line[] = "carol:!$6$hash:19000:0:99999:7:::";
  struct spwd entry;
  ASSERT_TRUE(parse(line, &entry));
  ASSERT_STREQ(entry.sp_pwdp, "!$6$hash");
}

TEST(LlvmLibcShadowTest, RejectsMalformed) {
  struct spwd entry;
  char too_few[] = "dave:hash:1:2";
  ASSERT_FALSE(parse(too_few, &entry));
  char not_a_number[] = "erin:hash:x:2:3:4:5:6:7";
  ASSERT_FALSE(parse(not_a_number, &entry));
  // Nine fields is the whole record, so a tenth means it is not one.
  char extra_field[] = "frank:hash:1:2:3:4:5:6:7:extra";
  ASSERT_FALSE(parse(extra_field, &entry));
}

TEST(LlvmLibcShadowTest, EmptyNameIsAllowed) {
  // The name is not checked for content, only for being a field. glibc
  // takes such a line too.
  char line[] = ":hash:1:2:3:4:5:6:7";
  struct spwd entry;
  ASSERT_TRUE(parse(line, &entry));
  ASSERT_STREQ(entry.sp_namp, "");
  ASSERT_STREQ(entry.sp_pwdp, "hash");
}
