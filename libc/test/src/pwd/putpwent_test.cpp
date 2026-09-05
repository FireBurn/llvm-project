//===-- Unittests for putpwent --------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/func/free.h"
#include "hdr/types/FILE.h"
#include "hdr/types/struct_passwd.h"
#include "src/pwd/putpwent.h"
#include "src/stdio/fclose.h"
#include "src/stdio/open_memstream.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

using LlvmLibcPutpwentTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Fails;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Succeeds;

// An entry goes out as the password file holds it: one line of seven colon
// separated fields, with a field that is not there left empty.
TEST_F(LlvmLibcPutpwentTest, WritesTheLineThePasswordFileHolds) {
  char *buffer = nullptr;
  size_t length = 0;
  ::FILE *stream = LIBC_NAMESPACE::open_memstream(&buffer, &length);
  ASSERT_FALSE(stream == nullptr);

  struct passwd root = {};
  root.pw_name = const_cast<char *>("root");
  root.pw_passwd = const_cast<char *>("x");
  root.pw_uid = 0;
  root.pw_gid = 0;
  root.pw_dir = const_cast<char *>("/root");
  root.pw_shell = const_cast<char *>("/bin/bash");
  ASSERT_THAT(LIBC_NAMESPACE::putpwent(&root, stream), Succeeds(0));
  ASSERT_EQ(LIBC_NAMESPACE::fclose(stream), 0);
  ASSERT_STREQ(buffer, "root:x:0:0::/root:/bin/bash\n");
  ::free(buffer);
}

TEST_F(LlvmLibcPutpwentTest, RefusesAnEntryWithNoName) {
  char *buffer = nullptr;
  size_t length = 0;
  ::FILE *stream = LIBC_NAMESPACE::open_memstream(&buffer, &length);
  ASSERT_FALSE(stream == nullptr);

  struct passwd nameless = {};
  ASSERT_THAT(LIBC_NAMESPACE::putpwent(&nameless, stream), Fails(EINVAL));
  ASSERT_EQ(LIBC_NAMESPACE::fclose(stream), 0);
  ::free(buffer);
}
