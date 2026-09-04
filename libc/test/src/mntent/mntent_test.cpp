//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the mntent family.
///
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/fcntl_macros.h"
#include "hdr/sys_stat_macros.h"
#include "hdr/types/FILE.h"
#include "hdr/types/struct_mntent.h"
#include "src/__support/CPP/string_view.h"
#include "src/fcntl/open.h"
#include "src/mntent/addmntent.h"
#include "src/mntent/endmntent.h"
#include "src/mntent/getmntent.h"
#include "src/mntent/getmntent_r.h"
#include "src/mntent/hasmntopt.h"
#include "src/mntent/setmntent.h"
#include "src/stdio/remove.h"
#include "src/unistd/close.h"
#include "src/unistd/write.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/Test.h"

#include <mntent.h>

using LlvmLibcMntentTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;
using string_view = LIBC_NAMESPACE::cpp::string_view;

namespace {

constexpr const char *TABLE = "mntent.test.fstab";

// A blank line, a comment, an entry whose mount point holds a space, and an
// entry with the trailing two numbers left off.
constexpr const char *CONTENTS = "\n"
                                 "# a comment\n"
                                 "/dev/sda1 / ext4 rw,relatime 0 1\n"
                                 "/dev/sda2 /mnt/with\\040space xfs ro 0 0\n"
                                 "proc /proc proc defaults\n";

void write_table(const char *contents) {
  int fd = LIBC_NAMESPACE::open(TABLE, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
  ASSERT_GT(fd, 0);
  string_view sv(contents);
  ASSERT_EQ(LIBC_NAMESPACE::write(fd, sv.data(), sv.size()),
            static_cast<ssize_t>(sv.size()));
  ASSERT_EQ(LIBC_NAMESPACE::close(fd), 0);
}

} // anonymous namespace

TEST_F(LlvmLibcMntentTest, ReadsEveryEntry) {
  write_table(CONTENTS);

  ::FILE *f = LIBC_NAMESPACE::setmntent(TABLE, "r");
  ASSERT_TRUE(f != nullptr);

  struct mntent *e = LIBC_NAMESPACE::getmntent(f);
  ASSERT_TRUE(e != nullptr);
  EXPECT_STREQ(e->mnt_fsname, "/dev/sda1");
  EXPECT_STREQ(e->mnt_dir, "/");
  EXPECT_STREQ(e->mnt_type, "ext4");
  EXPECT_STREQ(e->mnt_opts, "rw,relatime");
  EXPECT_EQ(e->mnt_freq, 0);
  EXPECT_EQ(e->mnt_passno, 1);

  // The octal escape for a space is undone.
  e = LIBC_NAMESPACE::getmntent(f);
  ASSERT_TRUE(e != nullptr);
  EXPECT_STREQ(e->mnt_dir, "/mnt/with space");
  EXPECT_STREQ(e->mnt_type, "xfs");

  // A line with no dump frequency or pass number gives zero for both.
  e = LIBC_NAMESPACE::getmntent(f);
  ASSERT_TRUE(e != nullptr);
  EXPECT_STREQ(e->mnt_fsname, "proc");
  EXPECT_EQ(e->mnt_freq, 0);
  EXPECT_EQ(e->mnt_passno, 0);

  EXPECT_TRUE(LIBC_NAMESPACE::getmntent(f) == nullptr);
  EXPECT_EQ(LIBC_NAMESPACE::endmntent(f), 1);

  ASSERT_EQ(LIBC_NAMESPACE::remove(TABLE), 0);
}

TEST_F(LlvmLibcMntentTest, ReentrantFormUsesTheCallersBuffer) {
  write_table(CONTENTS);

  ::FILE *f = LIBC_NAMESPACE::setmntent(TABLE, "r");
  ASSERT_TRUE(f != nullptr);

  struct mntent entry;
  char buffer[512];
  struct mntent *e =
      LIBC_NAMESPACE::getmntent_r(f, &entry, buffer, sizeof(buffer));
  ASSERT_TRUE(e == &entry);
  EXPECT_STREQ(entry.mnt_fsname, "/dev/sda1");

  EXPECT_EQ(LIBC_NAMESPACE::endmntent(f), 1);
  ASSERT_EQ(LIBC_NAMESPACE::remove(TABLE), 0);
}

TEST_F(LlvmLibcMntentTest, EmptyTable) {
  write_table("\n# only a comment\n");

  ::FILE *f = LIBC_NAMESPACE::setmntent(TABLE, "r");
  ASSERT_TRUE(f != nullptr);
  EXPECT_TRUE(LIBC_NAMESPACE::getmntent(f) == nullptr);
  EXPECT_EQ(LIBC_NAMESPACE::endmntent(f), 1);

  ASSERT_EQ(LIBC_NAMESPACE::remove(TABLE), 0);
}

TEST_F(LlvmLibcMntentTest, MissingFile) {
  EXPECT_TRUE(LIBC_NAMESPACE::setmntent("mntent.no.such.file", "r") == nullptr);
  ASSERT_ERRNO_EQ(ENOENT);
}

TEST_F(LlvmLibcMntentTest, WrittenEntryReadsBack) {
  write_table("");

  ::FILE *out = LIBC_NAMESPACE::setmntent(TABLE, "a");
  ASSERT_TRUE(out != nullptr);

  struct mntent entry;
  entry.mnt_fsname = const_cast<char *>("/dev/sdb1");
  entry.mnt_dir = const_cast<char *>("/mnt/a b");
  entry.mnt_type = const_cast<char *>("ext4");
  entry.mnt_opts = const_cast<char *>("rw,noatime");
  entry.mnt_freq = 1;
  entry.mnt_passno = 2;
  ASSERT_EQ(LIBC_NAMESPACE::addmntent(out, &entry), 0);
  ASSERT_EQ(LIBC_NAMESPACE::endmntent(out), 1);

  ::FILE *in = LIBC_NAMESPACE::setmntent(TABLE, "r");
  ASSERT_TRUE(in != nullptr);
  struct mntent *e = LIBC_NAMESPACE::getmntent(in);
  ASSERT_TRUE(e != nullptr);
  EXPECT_STREQ(e->mnt_fsname, "/dev/sdb1");
  // The space was written as an escape and read back as a space.
  EXPECT_STREQ(e->mnt_dir, "/mnt/a b");
  EXPECT_STREQ(e->mnt_opts, "rw,noatime");
  EXPECT_EQ(e->mnt_freq, 1);
  EXPECT_EQ(e->mnt_passno, 2);
  EXPECT_EQ(LIBC_NAMESPACE::endmntent(in), 1);

  ASSERT_EQ(LIBC_NAMESPACE::remove(TABLE), 0);
}

TEST_F(LlvmLibcMntentTest, HasMntOptMatchesWholeOptions) {
  struct mntent entry;
  entry.mnt_opts = const_cast<char *>("rw,relatime,errors=remount-ro");

  EXPECT_TRUE(LIBC_NAMESPACE::hasmntopt(&entry, "rw") == entry.mnt_opts);
  EXPECT_TRUE(LIBC_NAMESPACE::hasmntopt(&entry, "relatime") != nullptr);
  // A match on an option carrying a value returns the name, not the value.
  char *found = LIBC_NAMESPACE::hasmntopt(&entry, "errors");
  ASSERT_TRUE(found != nullptr);
  EXPECT_STREQ(found, "errors=remount-ro");

  // Part of a longer option is not a match.
  EXPECT_TRUE(LIBC_NAMESPACE::hasmntopt(&entry, "time") == nullptr);
  EXPECT_TRUE(LIBC_NAMESPACE::hasmntopt(&entry, "r") == nullptr);
  EXPECT_TRUE(LIBC_NAMESPACE::hasmntopt(&entry, "ro") == nullptr);
  EXPECT_TRUE(LIBC_NAMESPACE::hasmntopt(&entry, "noatime") == nullptr);
}

TEST_F(LlvmLibcMntentTest, HasMntOptOnAnEmptyList) {
  struct mntent entry;
  entry.mnt_opts = const_cast<char *>("");
  EXPECT_TRUE(LIBC_NAMESPACE::hasmntopt(&entry, "rw") == nullptr);
  entry.mnt_opts = nullptr;
  EXPECT_TRUE(LIBC_NAMESPACE::hasmntopt(&entry, "rw") == nullptr);
}
