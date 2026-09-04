//===-- Unittests for rewinddir, telldir and seekdir ----------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/__support/CPP/string_view.h"
#include "src/dirent/closedir.h"
#include "src/dirent/opendir.h"
#include "src/dirent/readdir.h"
#include "src/dirent/rewinddir.h"
#include "src/dirent/seekdir.h"
#include "src/dirent/telldir.h"

#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/Test.h"

#include <dirent.h>

using LlvmLibcRewinddirTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;
using string_view = LIBC_NAMESPACE::cpp::string_view;

namespace {

// A name has to be copied out before the next read, because readdir hands
// back a pointer into a buffer which the directory refills.
struct Name {
  char data[256] = {};

  bool empty() const { return data[0] == '\0'; }
  bool operator==(const Name &other) const {
    return string_view(data) == string_view(other.data);
  }
};

Name read_name(::DIR *dir) {
  Name name;
  struct ::dirent *d = LIBC_NAMESPACE::readdir(dir);
  if (d == nullptr)
    return name;
  string_view entry(&d->d_name[0]);
  size_t len = entry.size() < sizeof(name.data) - 1 ? entry.size()
                                                    : sizeof(name.data) - 1;
  for (size_t i = 0; i < len; ++i)
    name.data[i] = entry[i];
  return name;
}

} // anonymous namespace

TEST_F(LlvmLibcRewinddirTest, RewindRepeatsTheListing) {
  ::DIR *dir = LIBC_NAMESPACE::opendir("testdata");
  ASSERT_TRUE(dir != nullptr);

  Name first = read_name(dir);
  ASSERT_FALSE(first.empty());

  // Walk to the end so that the rewind has something to undo.
  size_t count = 1;
  while (LIBC_NAMESPACE::readdir(dir) != nullptr)
    ++count;

  LIBC_NAMESPACE::rewinddir(dir);
  ASSERT_ERRNO_SUCCESS();

  EXPECT_TRUE(read_name(dir) == first);
  size_t recount = 1;
  while (LIBC_NAMESPACE::readdir(dir) != nullptr)
    ++recount;
  EXPECT_EQ(recount, count);

  ASSERT_EQ(LIBC_NAMESPACE::closedir(dir), 0);
}

TEST_F(LlvmLibcRewinddirTest, SeekToAToldPosition) {
  ::DIR *dir = LIBC_NAMESPACE::opendir("testdata");
  ASSERT_TRUE(dir != nullptr);

  // A position taken before any read names the first entry.
  long start = LIBC_NAMESPACE::telldir(dir);
  Name first = read_name(dir);
  ASSERT_FALSE(first.empty());

  // And one taken after a read names the entry which follows it.
  long second_pos = LIBC_NAMESPACE::telldir(dir);
  Name second = read_name(dir);
  ASSERT_FALSE(second.empty());
  ASSERT_FALSE(second == first);

  LIBC_NAMESPACE::seekdir(dir, second_pos);
  ASSERT_ERRNO_SUCCESS();
  EXPECT_TRUE(read_name(dir) == second);

  LIBC_NAMESPACE::seekdir(dir, start);
  EXPECT_TRUE(read_name(dir) == first);

  ASSERT_EQ(LIBC_NAMESPACE::closedir(dir), 0);
}
