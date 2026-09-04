//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for parse_group_line.
///
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/types/struct_group.h"
#include "src/grp/grp_utils.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcGrpTest, ParseGroupLine_Success) {
  char line[] = "wheel:x:10:root,alice,bob";
  auto res = LIBC_NAMESPACE::grp::parse_group_line(line);
  ASSERT_TRUE(res.has_value());
  struct group grp = res.value();
  ASSERT_STREQ(grp.gr_name, "wheel");
  ASSERT_STREQ(grp.gr_passwd, "x");
  ASSERT_EQ(grp.gr_gid, 10u);
  ASSERT_STREQ(grp.gr_mem[0], "root");
  ASSERT_STREQ(grp.gr_mem[1], "alice");
  ASSERT_STREQ(grp.gr_mem[2], "bob");
  ASSERT_TRUE(grp.gr_mem[3] == nullptr);
}

TEST(LlvmLibcGrpTest, ParseGroupLine_NoMembers) {
  char line[] = "nogroup:x:65534:";
  auto res = LIBC_NAMESPACE::grp::parse_group_line(line);
  ASSERT_TRUE(res.has_value());
  struct group grp = res.value();
  ASSERT_STREQ(grp.gr_name, "nogroup");
  ASSERT_EQ(grp.gr_gid, 65534u);
  ASSERT_TRUE(grp.gr_mem[0] == nullptr);
}

TEST(LlvmLibcGrpTest, ParseGroupLine_OneMember) {
  char line[] = "adm:x:4:syslog";
  auto res = LIBC_NAMESPACE::grp::parse_group_line(line);
  ASSERT_TRUE(res.has_value());
  struct group grp = res.value();
  ASSERT_EQ(grp.gr_gid, 4u);
  ASSERT_STREQ(grp.gr_mem[0], "syslog");
  ASSERT_TRUE(grp.gr_mem[1] == nullptr);
}

TEST(LlvmLibcGrpTest, ParseGroupLine_EmptyPassword) {
  char line[] = "users::100:";
  auto res = LIBC_NAMESPACE::grp::parse_group_line(line);
  ASSERT_TRUE(res.has_value());
  struct group grp = res.value();
  ASSERT_STREQ(grp.gr_passwd, "");
  ASSERT_EQ(grp.gr_gid, 100u);
}

TEST(LlvmLibcGrpTest, ParseGroupLine_MissingFields) {
  char line[] = "wheel:x";
  auto res = LIBC_NAMESPACE::grp::parse_group_line(line);
  ASSERT_FALSE(res.has_value());
}

TEST(LlvmLibcGrpTest, ParseGroupLine_NonNumericGid) {
  char line[] = "wheel:x:ten:root";
  auto res = LIBC_NAMESPACE::grp::parse_group_line(line);
  ASSERT_FALSE(res.has_value());
}

TEST(LlvmLibcGrpTest, ParseGroupLine_Null) {
  auto res = LIBC_NAMESPACE::grp::parse_group_line(nullptr);
  ASSERT_FALSE(res.has_value());
  ASSERT_EQ(res.error(), EINVAL);
}
