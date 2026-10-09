//===-- Unittests for readdir_r ---------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for readdir_r.
///
//===----------------------------------------------------------------------===//

#include "hdr/types/struct_dirent.h"
#include "src/dirent/closedir.h"
#include "src/dirent/opendir.h"
#include "src/dirent/readdir_r.h"
#include "src/string/strcmp.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/Test.h"

using LlvmLibcReaddirRTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;

TEST_F(LlvmLibcReaddirRTest, ReadsEveryEntryAndThenNull) {
  ::DIR *dir = LIBC_NAMESPACE::opendir(".");
  ASSERT_TRUE(dir != nullptr);

  struct dirent storage;
  struct dirent *result = nullptr;
  bool found_dot = false;
  bool found_dotdot = false;
  int count = 0;
  for (;;) {
    // Unlike readdir, the result comes back as the return value, and the end
    // of the directory is a null result with no error.
    ASSERT_EQ(LIBC_NAMESPACE::readdir_r(dir, &storage, &result), 0);
    if (result == nullptr)
      break;
    ASSERT_TRUE(result == &storage);
    if (LIBC_NAMESPACE::strcmp(storage.d_name, ".") == 0)
      found_dot = true;
    if (LIBC_NAMESPACE::strcmp(storage.d_name, "..") == 0)
      found_dotdot = true;
    ++count;
  }
  ASSERT_GE(count, 2);
  ASSERT_TRUE(found_dot);
  ASSERT_TRUE(found_dotdot);
  ASSERT_ERRNO_SUCCESS();
  ASSERT_EQ(LIBC_NAMESPACE::closedir(dir), 0);
}
