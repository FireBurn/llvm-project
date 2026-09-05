//===-- Unittests for putgrent --------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/func/free.h"
#include "hdr/types/FILE.h"
#include "hdr/types/struct_group.h"
#include "src/grp/putgrent.h"
#include "src/stdio/fclose.h"
#include "src/stdio/open_memstream.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

using LlvmLibcPutgrentTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Fails;

namespace {

// Writes one group out through putgrent and hands back what it returned,
// with the line it became left in *buffer.
int write_group(const struct group &g, char **buffer) {
  size_t length = 0;
  ::FILE *stream = LIBC_NAMESPACE::open_memstream(buffer, &length);
  if (stream == nullptr)
    return -2;
  int result = LIBC_NAMESPACE::putgrent(&g, stream);
  if (LIBC_NAMESPACE::fclose(stream) != 0)
    return -3;
  return result;
}

} // anonymous namespace

// An entry goes out as the group file holds it, with the members separated
// by commas.
TEST_F(LlvmLibcPutgrentTest, WritesTheLineTheGroupFileHolds) {
  char *members[] = {const_cast<char *>("root"), const_cast<char *>("daemon"),
                     nullptr};
  struct group wheel = {};
  wheel.gr_name = const_cast<char *>("wheel");
  wheel.gr_passwd = const_cast<char *>("x");
  wheel.gr_gid = 10;
  wheel.gr_mem = members;

  char *buffer = nullptr;
  ASSERT_EQ(write_group(wheel, &buffer), 0);
  ASSERT_STREQ(buffer, "wheel:x:10:root,daemon\n");
  ::free(buffer);
}

// A group with no members, or no password, still has every field, and the
// last of them is empty.
TEST_F(LlvmLibcPutgrentTest, LeavesAnEmptyFieldForWhatIsNotThere) {
  struct group users = {};
  users.gr_name = const_cast<char *>("users");
  users.gr_gid = 100;

  char *buffer = nullptr;
  ASSERT_EQ(write_group(users, &buffer), 0);
  ASSERT_STREQ(buffer, "users::100:\n");
  ::free(buffer);
}

TEST_F(LlvmLibcPutgrentTest, RefusesAnEntryWithNoName) {
  char *buffer = nullptr;
  size_t length = 0;
  ::FILE *stream = LIBC_NAMESPACE::open_memstream(&buffer, &length);
  ASSERT_FALSE(stream == nullptr);

  struct group nameless = {};
  ASSERT_THAT(LIBC_NAMESPACE::putgrent(&nameless, stream), Fails(EINVAL));
  ASSERT_EQ(LIBC_NAMESPACE::fclose(stream), 0);
  ::free(buffer);
}
