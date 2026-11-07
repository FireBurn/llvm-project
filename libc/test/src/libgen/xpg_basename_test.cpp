//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for __xpg_basename.
///
//===----------------------------------------------------------------------===//

#include "src/libgen/xpg_basename.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcXpgBasenameTest, NullPointer) {
  ASSERT_STREQ(LIBC_NAMESPACE::__xpg_basename(nullptr), ".");
}

TEST(LlvmLibcXpgBasenameTest, EmptyString) {
  char path[] = "";
  ASSERT_STREQ(LIBC_NAMESPACE::__xpg_basename(path), ".");
}

TEST(LlvmLibcXpgBasenameTest, RegularPath) {
  char path[] = "/usr/lib";
  ASSERT_STREQ(LIBC_NAMESPACE::__xpg_basename(path), "lib");
}

TEST(LlvmLibcXpgBasenameTest, TrailingSlash) {
  char path[] = "/usr/";
  ASSERT_STREQ(LIBC_NAMESPACE::__xpg_basename(path), "usr");
  ASSERT_STREQ(path, "/usr");
}

TEST(LlvmLibcXpgBasenameTest, SingleSlash) {
  char path[] = "/";
  ASSERT_STREQ(LIBC_NAMESPACE::__xpg_basename(path), "/");
}

TEST(LlvmLibcXpgBasenameTest, MultipleSlashes) {
  char path[] = "///";
  ASSERT_STREQ(LIBC_NAMESPACE::__xpg_basename(path), "/");
}

TEST(LlvmLibcXpgBasenameTest, SimpleName) {
  char path[] = "a";
  ASSERT_STREQ(LIBC_NAMESPACE::__xpg_basename(path), "a");
}

TEST(LlvmLibcXpgBasenameTest, SimpleNameTrailingSlash) {
  char path[] = "a/";
  ASSERT_STREQ(LIBC_NAMESPACE::__xpg_basename(path), "a");
  ASSERT_STREQ(path, "a");
}

TEST(LlvmLibcXpgBasenameTest, ComplexPath) {
  char path[] = "///a///";
  ASSERT_STREQ(LIBC_NAMESPACE::__xpg_basename(path), "a");
  ASSERT_STREQ(path, "///a");
}
