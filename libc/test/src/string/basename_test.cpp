//===-- Unittests for the GNU basename ------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/string/basename.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcGnuBasenameTest, PlainName) {
  const char *path = "name";
  ASSERT_STREQ(LIBC_NAMESPACE::basename(path), "name");
  ASSERT_TRUE(LIBC_NAMESPACE::basename(path) == path);
}

TEST(LlvmLibcGnuBasenameTest, LastComponent) {
  ASSERT_STREQ(LIBC_NAMESPACE::basename("/usr/lib"), "lib");
  ASSERT_STREQ(LIBC_NAMESPACE::basename("a/b/c.txt"), "c.txt");
}

TEST(LlvmLibcGnuBasenameTest, TrailingSlashGivesAnEmptyString) {
  ASSERT_STREQ(LIBC_NAMESPACE::basename("/usr/"), "");
  ASSERT_STREQ(LIBC_NAMESPACE::basename("/"), "");
}

TEST(LlvmLibcGnuBasenameTest, EmptyPath) {
  ASSERT_STREQ(LIBC_NAMESPACE::basename(""), "");
}

TEST(LlvmLibcGnuBasenameTest, DoesNotChangeItsArgument) {
  char path[] = "/usr/lib/";
  LIBC_NAMESPACE::basename(path);
  ASSERT_STREQ(path, "/usr/lib/");
}
