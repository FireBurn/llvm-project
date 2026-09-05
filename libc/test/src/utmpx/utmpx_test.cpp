//===-- Unittests for the utmpx interface ---------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/fcntl_macros.h"
#include "hdr/sys_stat_macros.h"
#include "hdr/types/struct_utmpx.h"
#include "src/fcntl/open.h"
#include "src/stdio/remove.h"
#include "src/string/strcpy.h"
#include "src/string/strncmp.h"
#include "src/unistd/close.h"
#include "src/utmpx/endutxent.h"
#include "src/utmpx/getutxent.h"
#include "src/utmpx/getutxid.h"
#include "src/utmpx/getutxline.h"
#include "src/utmpx/pututxline.h"
#include "src/utmpx/setutxent.h"
#include "src/utmpx/updwtmpx.h"
#include "src/utmpx/utmpxname.h"
#include "test/UnitTest/Test.h"

#include <utmpx.h>

namespace {

constexpr const char *TEST_FILE = "utmpx.test.db";

struct utmpx record(short type, pid_t pid, const char *line, const char *id,
                    const char *user) {
  struct utmpx entry = {};
  entry.ut_type = type;
  entry.ut_pid = pid;
  LIBC_NAMESPACE::strcpy(entry.ut_line, line);
  for (int i = 0; i < 4; ++i)
    entry.ut_id[i] = id[i];
  LIBC_NAMESPACE::strcpy(entry.ut_user, user);
  entry.ut_tv.tv_sec = 1000;
  return entry;
}

bool named(const struct utmpx *entry, const char *user) {
  return entry != nullptr &&
         LIBC_NAMESPACE::strncmp(entry->ut_user, user, __UT_NAMESIZE) == 0;
}

} // anonymous namespace

class LlvmLibcUtmpxTest : public LIBC_NAMESPACE::testing::Test {
public:
  void SetUp() override {
    ASSERT_EQ(LIBC_NAMESPACE::utmpxname(TEST_FILE), 0);
    LIBC_NAMESPACE::endutxent();
    // The calls open the file but do not create it, so each test starts
    // from an empty one of its own.
    int fd =
        LIBC_NAMESPACE::open(TEST_FILE, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
    ASSERT_GT(fd, 0);
    ASSERT_EQ(LIBC_NAMESPACE::close(fd), 0);
  }

  void TearDown() override {
    LIBC_NAMESPACE::endutxent();
    LIBC_NAMESPACE::remove(TEST_FILE);
  }
};

TEST_F(LlvmLibcUtmpxTest, TheRecordIsTheSameSizeAsGlibcs) {
  // The file is shared with other programs, so the layout is fixed, and it
  // is the same one utmp uses.
  EXPECT_EQ(sizeof(struct utmpx), size_t(384));
}

TEST_F(LlvmLibcUtmpxTest, WritesAndReadsBackInOrder) {
  struct utmpx first = record(USER_PROCESS, 4321, "pts/7", "ts/7", "alice");
  struct utmpx second = record(USER_PROCESS, 8765, "pts/8", "ts/8", "bob");

  LIBC_NAMESPACE::setutxent();
  ASSERT_TRUE(LIBC_NAMESPACE::pututxline(&first) != nullptr);
  ASSERT_TRUE(LIBC_NAMESPACE::pututxline(&second) != nullptr);
  LIBC_NAMESPACE::endutxent();

  LIBC_NAMESPACE::setutxent();
  EXPECT_TRUE(named(LIBC_NAMESPACE::getutxent(), "alice"));
  EXPECT_TRUE(named(LIBC_NAMESPACE::getutxent(), "bob"));
  EXPECT_TRUE(LIBC_NAMESPACE::getutxent() == nullptr);
  LIBC_NAMESPACE::endutxent();
}

TEST_F(LlvmLibcUtmpxTest, FindsARecordByLineAndById) {
  struct utmpx first = record(USER_PROCESS, 4321, "pts/7", "ts/7", "alice");
  struct utmpx second = record(USER_PROCESS, 8765, "pts/8", "ts/8", "bob");

  LIBC_NAMESPACE::setutxent();
  LIBC_NAMESPACE::pututxline(&first);
  LIBC_NAMESPACE::pututxline(&second);
  LIBC_NAMESPACE::endutxent();

  struct utmpx line = {};
  LIBC_NAMESPACE::strcpy(line.ut_line, "pts/8");
  LIBC_NAMESPACE::setutxent();
  EXPECT_TRUE(named(LIBC_NAMESPACE::getutxline(&line), "bob"));
  LIBC_NAMESPACE::endutxent();

  struct utmpx id = {};
  id.ut_type = USER_PROCESS;
  for (int i = 0; i < 4; ++i)
    id.ut_id[i] = "ts/7"[i];
  LIBC_NAMESPACE::setutxent();
  EXPECT_TRUE(named(LIBC_NAMESPACE::getutxid(&id), "alice"));
  LIBC_NAMESPACE::endutxent();

  // A key matching nothing walks to the end and reports it.
  struct utmpx absent = {};
  LIBC_NAMESPACE::strcpy(absent.ut_line, "pts/99");
  LIBC_NAMESPACE::setutxent();
  EXPECT_TRUE(LIBC_NAMESPACE::getutxline(&absent) == nullptr);
  LIBC_NAMESPACE::endutxent();
}

TEST_F(LlvmLibcUtmpxTest, ReplacesTheRecordWithTheSameId) {
  struct utmpx live = record(USER_PROCESS, 8765, "pts/8", "ts/8", "bob");
  LIBC_NAMESPACE::setutxent();
  LIBC_NAMESPACE::pututxline(&live);
  LIBC_NAMESPACE::endutxent();

  struct utmpx dead = record(DEAD_PROCESS, 8765, "pts/8", "ts/8", "");
  LIBC_NAMESPACE::setutxent();
  LIBC_NAMESPACE::pututxline(&dead);
  LIBC_NAMESPACE::endutxent();

  LIBC_NAMESPACE::setutxent();
  struct utmpx *entry = LIBC_NAMESPACE::getutxent();
  ASSERT_TRUE(entry != nullptr);
  EXPECT_EQ(int(entry->ut_type), int(DEAD_PROCESS));
  EXPECT_TRUE(LIBC_NAMESPACE::getutxent() == nullptr);
  LIBC_NAMESPACE::endutxent();
}

TEST_F(LlvmLibcUtmpxTest, AppendsToTheLoginHistory) {
  constexpr const char *HISTORY_FILE = "wtmpx.test.db";
  // The history is appended to and never created, as glibc's is, so it has
  // to be there before anything is written to it.
  int fd =
      LIBC_NAMESPACE::open(HISTORY_FILE, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
  ASSERT_GT(fd, 0);
  ASSERT_EQ(LIBC_NAMESPACE::close(fd), 0);

  struct utmpx first = record(USER_PROCESS, 4321, "pts/7", "ts/7", "alice");
  LIBC_NAMESPACE::updwtmpx(HISTORY_FILE, &first);

  // The history holds the same records, so the ordinary walk reads it.
  ASSERT_EQ(LIBC_NAMESPACE::utmpxname(HISTORY_FILE), 0);
  LIBC_NAMESPACE::setutxent();
  EXPECT_TRUE(named(LIBC_NAMESPACE::getutxent(), "alice"));
  LIBC_NAMESPACE::endutxent();

  LIBC_NAMESPACE::remove(HISTORY_FILE);
  ASSERT_EQ(LIBC_NAMESPACE::utmpxname(TEST_FILE), 0);
}
