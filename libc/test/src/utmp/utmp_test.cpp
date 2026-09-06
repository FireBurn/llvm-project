//===-- Unittests for the utmp interface ----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/fcntl_macros.h"
#include "hdr/sys_stat_macros.h"
#include "hdr/types/struct_utmp.h"
#include "src/__support/CPP/string_view.h"
#include "src/fcntl/open.h"
#include "src/stdio/remove.h"
#include "src/string/strcpy.h"
#include "src/string/strncmp.h"
#include "src/unistd/close.h"
#include "src/utmp/endutent.h"
#include "src/utmp/getutent.h"
#include "src/utmp/getutent_r.h"
#include "src/utmp/getutid.h"
#include "src/utmp/getutid_r.h"
#include "src/utmp/getutline.h"
#include "src/utmp/getutline_r.h"
#include "src/utmp/logout.h"
#include "src/utmp/logwtmp.h"
#include "src/utmp/pututline.h"
#include "src/utmp/setutent.h"
#include "src/utmp/updwtmp.h"
#include "src/utmp/utmpname.h"
#include "test/UnitTest/Test.h"

namespace {

constexpr const char *TEST_FILE = "utmp.test.db";

struct utmp record(short type, pid_t pid, const char *line, const char *id,
                   const char *user) {
  struct utmp entry = {};
  entry.ut_type = type;
  entry.ut_pid = pid;
  LIBC_NAMESPACE::strcpy(entry.ut_line, line);
  for (int i = 0; i < 4; ++i)
    entry.ut_id[i] = id[i];
  LIBC_NAMESPACE::strcpy(entry.ut_user, user);
  entry.ut_tv.tv_sec = 1000;
  return entry;
}

bool named(const struct utmp *entry, const char *user) {
  return entry != nullptr &&
         LIBC_NAMESPACE::strncmp(entry->ut_user, user, UT_NAMESIZE) == 0;
}

} // anonymous namespace

class LlvmLibcUtmpTest : public LIBC_NAMESPACE::testing::Test {
public:
  void SetUp() override {
    ASSERT_EQ(LIBC_NAMESPACE::utmpname(TEST_FILE), 0);
    LIBC_NAMESPACE::endutent();
    // The calls open the file but do not create it, the same as glibc's, so
    // each test starts from an empty one of its own.
    int fd =
        LIBC_NAMESPACE::open(TEST_FILE, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
    ASSERT_GT(fd, 0);
    ASSERT_EQ(LIBC_NAMESPACE::close(fd), 0);
  }

  void TearDown() override {
    LIBC_NAMESPACE::endutent();
    LIBC_NAMESPACE::remove(TEST_FILE);
  }
};

TEST_F(LlvmLibcUtmpTest, TheRecordIsTheSameSizeAsGlibcs) {
  // The file is shared with other programs, so the layout is fixed.
  EXPECT_EQ(sizeof(struct utmp), size_t(384));
  EXPECT_EQ(UT_LINESIZE, 32);
  EXPECT_EQ(UT_NAMESIZE, 32);
  EXPECT_EQ(UT_HOSTSIZE, 256);
}

TEST_F(LlvmLibcUtmpTest, WritesAndReadsBackInOrder) {
  struct utmp first = record(USER_PROCESS, 4321, "pts/7", "ts/7", "alice");
  struct utmp second = record(USER_PROCESS, 8765, "pts/8", "ts/8", "bob");

  LIBC_NAMESPACE::setutent();
  ASSERT_TRUE(LIBC_NAMESPACE::pututline(&first) != nullptr);
  ASSERT_TRUE(LIBC_NAMESPACE::pututline(&second) != nullptr);
  LIBC_NAMESPACE::endutent();

  LIBC_NAMESPACE::setutent();
  EXPECT_TRUE(named(LIBC_NAMESPACE::getutent(), "alice"));
  EXPECT_TRUE(named(LIBC_NAMESPACE::getutent(), "bob"));
  EXPECT_TRUE(LIBC_NAMESPACE::getutent() == nullptr);
  LIBC_NAMESPACE::endutent();
}

TEST_F(LlvmLibcUtmpTest, FindsARecordByLineAndById) {
  struct utmp first = record(USER_PROCESS, 4321, "pts/7", "ts/7", "alice");
  struct utmp second = record(USER_PROCESS, 8765, "pts/8", "ts/8", "bob");

  LIBC_NAMESPACE::setutent();
  LIBC_NAMESPACE::pututline(&first);
  LIBC_NAMESPACE::pututline(&second);
  LIBC_NAMESPACE::endutent();

  struct utmp key = {};
  LIBC_NAMESPACE::strcpy(key.ut_line, "pts/8");
  LIBC_NAMESPACE::setutent();
  EXPECT_TRUE(named(LIBC_NAMESPACE::getutline(&key), "bob"));
  LIBC_NAMESPACE::endutent();

  struct utmp id = {};
  id.ut_type = USER_PROCESS;
  for (int i = 0; i < 4; ++i)
    id.ut_id[i] = "ts/7"[i];
  LIBC_NAMESPACE::setutent();
  EXPECT_TRUE(named(LIBC_NAMESPACE::getutid(&id), "alice"));
  LIBC_NAMESPACE::endutent();
}

TEST_F(LlvmLibcUtmpTest, ReplacesTheRecordWithTheSameId) {
  struct utmp live = record(USER_PROCESS, 8765, "pts/8", "ts/8", "bob");
  LIBC_NAMESPACE::setutent();
  LIBC_NAMESPACE::pututline(&live);
  LIBC_NAMESPACE::endutent();

  struct utmp dead = record(DEAD_PROCESS, 8765, "pts/8", "ts/8", "");
  LIBC_NAMESPACE::setutent();
  LIBC_NAMESPACE::pututline(&dead);
  LIBC_NAMESPACE::endutent();

  // Replaced rather than appended, so there is still only the one record.
  LIBC_NAMESPACE::setutent();
  struct utmp *entry = LIBC_NAMESPACE::getutent();
  ASSERT_TRUE(entry != nullptr);
  EXPECT_EQ(int(entry->ut_type), int(DEAD_PROCESS));
  EXPECT_TRUE(LIBC_NAMESPACE::getutent() == nullptr);
  LIBC_NAMESPACE::endutent();
}

TEST_F(LlvmLibcUtmpTest, CopiesTheRecordOutForTheReentrantForm) {
  struct utmp first = record(USER_PROCESS, 4321, "pts/7", "ts/7", "alice");
  LIBC_NAMESPACE::setutent();
  LIBC_NAMESPACE::pututline(&first);
  LIBC_NAMESPACE::endutent();

  struct utmp buffer = {};
  struct utmp *result = nullptr;
  LIBC_NAMESPACE::setutent();
  EXPECT_EQ(LIBC_NAMESPACE::getutent_r(&buffer, &result), 0);
  EXPECT_TRUE(result == &buffer);
  EXPECT_TRUE(named(result, "alice"));
  // Nothing left, and the caller's pointer says so.
  EXPECT_EQ(LIBC_NAMESPACE::getutent_r(&buffer, &result), -1);
  EXPECT_TRUE(result == nullptr);
  LIBC_NAMESPACE::endutent();
}

TEST_F(LlvmLibcUtmpTest, CopiesOutTheRecordFoundByIdAndByLine) {
  struct utmp first = record(USER_PROCESS, 4321, "pts/7", "ts/7", "alice");
  struct utmp second = record(USER_PROCESS, 8765, "pts/8", "ts/8", "bob");
  LIBC_NAMESPACE::setutent();
  LIBC_NAMESPACE::pututline(&first);
  LIBC_NAMESPACE::pututline(&second);
  LIBC_NAMESPACE::endutent();

  struct utmp buffer = {};
  struct utmp *result = nullptr;

  struct utmp id = {};
  id.ut_type = USER_PROCESS;
  for (int i = 0; i < 4; ++i)
    id.ut_id[i] = "ts/7"[i];
  LIBC_NAMESPACE::setutent();
  EXPECT_EQ(LIBC_NAMESPACE::getutid_r(&id, &buffer, &result), 0);
  EXPECT_TRUE(result == &buffer);
  EXPECT_TRUE(named(result, "alice"));
  LIBC_NAMESPACE::endutent();

  struct utmp line = {};
  LIBC_NAMESPACE::strcpy(line.ut_line, "pts/8");
  LIBC_NAMESPACE::setutent();
  EXPECT_EQ(LIBC_NAMESPACE::getutline_r(&line, &buffer, &result), 0);
  EXPECT_TRUE(result == &buffer);
  EXPECT_TRUE(named(result, "bob"));
  LIBC_NAMESPACE::endutent();

  // A key matching nothing is reported through the caller's pointer.
  struct utmp absent = {};
  LIBC_NAMESPACE::strcpy(absent.ut_line, "pts/99");
  LIBC_NAMESPACE::setutent();
  EXPECT_EQ(LIBC_NAMESPACE::getutline_r(&absent, &buffer, &result), -1);
  EXPECT_TRUE(result == nullptr);
  LIBC_NAMESPACE::endutent();
}

TEST_F(LlvmLibcUtmpTest, LogoutMarksTheRecordDead) {
  struct utmp live = record(USER_PROCESS, 8765, "pts/8", "ts/8", "bob");
  LIBC_NAMESPACE::setutent();
  LIBC_NAMESPACE::pututline(&live);
  LIBC_NAMESPACE::endutent();

  EXPECT_EQ(LIBC_NAMESPACE::logout("pts/8"), 1);

  LIBC_NAMESPACE::setutent();
  struct utmp *entry = LIBC_NAMESPACE::getutent();
  ASSERT_TRUE(entry != nullptr);
  EXPECT_EQ(int(entry->ut_type), int(DEAD_PROCESS));
  EXPECT_EQ(entry->ut_user[0], '\0');
  LIBC_NAMESPACE::endutent();

  // A terminal with no record is reported rather than invented.
  EXPECT_EQ(LIBC_NAMESPACE::logout("pts/99"), 0);
}

TEST_F(LlvmLibcUtmpTest, AppendsToTheLoginHistory) {
  constexpr const char *HISTORY_FILE = "wtmp.test.db";
  // The history is appended to and never created, as glibc's is, so it has
  // to be there before anything is written to it.
  int fd =
      LIBC_NAMESPACE::open(HISTORY_FILE, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
  ASSERT_GT(fd, 0);
  ASSERT_EQ(LIBC_NAMESPACE::close(fd), 0);

  struct utmp first = record(USER_PROCESS, 4321, "pts/7", "ts/7", "alice");
  LIBC_NAMESPACE::updwtmp(HISTORY_FILE, &first);
  LIBC_NAMESPACE::logwtmp("pts/8", "bob", "elsewhere");

  // The history is read by pointing the ordinary walk at it, since it holds
  // the same records.
  ASSERT_EQ(LIBC_NAMESPACE::utmpname(HISTORY_FILE), 0);
  LIBC_NAMESPACE::setutent();
  EXPECT_TRUE(named(LIBC_NAMESPACE::getutent(), "alice"));
  LIBC_NAMESPACE::endutent();

  LIBC_NAMESPACE::remove(HISTORY_FILE);
  ASSERT_EQ(LIBC_NAMESPACE::utmpname(TEST_FILE), 0);
}
