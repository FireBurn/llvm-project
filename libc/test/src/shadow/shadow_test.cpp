//===-- Unittests for the shadow password database ------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/types/FILE.h"
#include "hdr/types/struct_spwd.h"
#include "src/__support/libc_errno.h"
#include "src/shadow/endspent.h"
#include "src/shadow/fgetspent.h"
#include "src/shadow/fgetspent_r.h"
#include "src/shadow/getspent.h"
#include "src/shadow/getspent_r.h"
#include "src/shadow/getspnam.h"
#include "src/shadow/getspnam_r.h"
#include "src/shadow/lckpwdf.h"
#include "src/shadow/putspent.h"
#include "src/shadow/setspent.h"
#include "src/shadow/sgetspent.h"
#include "src/shadow/sgetspent_r.h"
#include "src/shadow/shadow_utils.h"
#include "src/shadow/ulckpwdf.h"
#include "src/stdio/fclose.h"
#include "src/stdio/fopen.h"
#include "src/stdio/fwrite.h"
#include "src/stdio/remove.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/Test.h"

namespace {

constexpr char FILE_CONTENTS[] = "alice:$y$alice:19500:1:180:14:30:20000:0\n"
                                 "bob:!:19600:0:99999:7:::\n";

// The database is read from whatever path is set, so each test points it at
// a file it wrote itself rather than at the machine's own.
class LlvmLibcShadowTest : public LIBC_NAMESPACE::testing::ErrnoCheckingTest {
public:
  void SetUp() override {
    LIBC_NAMESPACE::testing::ErrnoCheckingTest::SetUp();
    path = libc_make_test_file_path("shadow_test.shadow");
    ::FILE *f = LIBC_NAMESPACE::fopen(path, "w");
    ASSERT_FALSE(f == nullptr);
    ASSERT_EQ(
        LIBC_NAMESPACE::fwrite(FILE_CONTENTS, 1, sizeof(FILE_CONTENTS) - 1, f),
        sizeof(FILE_CONTENTS) - 1);
    ASSERT_EQ(LIBC_NAMESPACE::fclose(f), 0);
    LIBC_NAMESPACE::shadow_db::TESTONLY_set_shadow_path(path);
  }

  void TearDown() override {
    LIBC_NAMESPACE::endspent();
    LIBC_NAMESPACE::remove(path);
    LIBC_NAMESPACE::testing::ErrnoCheckingTest::TearDown();
  }

  const char *path;
};

} // anonymous namespace

TEST_F(LlvmLibcShadowTest, LookUpByName) {
  struct spwd *entry = LIBC_NAMESPACE::getspnam("bob");
  ASSERT_FALSE(entry == nullptr);
  ASSERT_STREQ(entry->sp_namp, "bob");
  ASSERT_STREQ(entry->sp_pwdp, "!");
  ASSERT_EQ(entry->sp_lstchg, 19600L);

  // A field the file left empty reads as -1.
  ASSERT_EQ(entry->sp_inact, -1L);
  ASSERT_EQ(entry->sp_expire, -1L);
}

TEST_F(LlvmLibcShadowTest, NameThatIsNotThere) {
  ASSERT_TRUE(LIBC_NAMESPACE::getspnam("nobody") == nullptr);
}

TEST_F(LlvmLibcShadowTest, WalkTheWholeFile) {
  LIBC_NAMESPACE::setspent();

  struct spwd *first = LIBC_NAMESPACE::getspent();
  ASSERT_FALSE(first == nullptr);
  ASSERT_STREQ(first->sp_namp, "alice");

  struct spwd *second = LIBC_NAMESPACE::getspent();
  ASSERT_FALSE(second == nullptr);
  ASSERT_STREQ(second->sp_namp, "bob");

  ASSERT_TRUE(LIBC_NAMESPACE::getspent() == nullptr);

  // Rewinding starts the walk again.
  LIBC_NAMESPACE::setspent();
  struct spwd *again = LIBC_NAMESPACE::getspent();
  ASSERT_FALSE(again == nullptr);
  ASSERT_STREQ(again->sp_namp, "alice");
}

TEST_F(LlvmLibcShadowTest, ReadFromAStream) {
  ::FILE *f = LIBC_NAMESPACE::fopen(path, "r");
  ASSERT_FALSE(f == nullptr);

  struct spwd *first = LIBC_NAMESPACE::fgetspent(f);
  ASSERT_FALSE(first == nullptr);
  ASSERT_STREQ(first->sp_namp, "alice");

  struct spwd *second = LIBC_NAMESPACE::fgetspent(f);
  ASSERT_FALSE(second == nullptr);
  ASSERT_STREQ(second->sp_namp, "bob");

  ASSERT_TRUE(LIBC_NAMESPACE::fgetspent(f) == nullptr);
  ASSERT_EQ(LIBC_NAMESPACE::fclose(f), 0);
}

TEST_F(LlvmLibcShadowTest, ReadFromAString) {
  struct spwd *entry =
      LIBC_NAMESPACE::sgetspent("carol:x:19000:2:90:7:14:21000:1");
  ASSERT_FALSE(entry == nullptr);
  ASSERT_STREQ(entry->sp_namp, "carol");
  ASSERT_EQ(entry->sp_min, 2L);
  ASSERT_EQ(entry->sp_flag, 1UL);

  // A line that is not an entry is reported as glibc reports it.
  ASSERT_TRUE(LIBC_NAMESPACE::sgetspent("not a shadow line") == nullptr);
  ASSERT_ERRNO_EQ(EINVAL);
  LIBC_NAMESPACE::libc_errno = 0;
}

TEST_F(LlvmLibcShadowTest, CallerSuppliedBuffers) {
  struct spwd entry;
  char buf[256];
  struct spwd *result = nullptr;

  ASSERT_EQ(
      LIBC_NAMESPACE::getspnam_r("alice", &entry, buf, sizeof(buf), &result),
      0);
  ASSERT_TRUE(result == &entry);
  ASSERT_STREQ(entry.sp_namp, "alice");

  // A name that is not there is not an error: the result is just empty.
  ASSERT_EQ(
      LIBC_NAMESPACE::getspnam_r("nobody", &entry, buf, sizeof(buf), &result),
      0);
  ASSERT_TRUE(result == nullptr);

  LIBC_NAMESPACE::setspent();
  ASSERT_EQ(LIBC_NAMESPACE::getspent_r(&entry, buf, sizeof(buf), &result), 0);
  ASSERT_TRUE(result == &entry);
  ASSERT_STREQ(entry.sp_namp, "alice");

  ASSERT_EQ(LIBC_NAMESPACE::sgetspent_r("dave:x:1:2:3:4:5:6:7", &entry, buf,
                                        sizeof(buf), &result),
            0);
  ASSERT_TRUE(result == &entry);
  ASSERT_STREQ(entry.sp_namp, "dave");

  ::FILE *f = LIBC_NAMESPACE::fopen(path, "r");
  ASSERT_FALSE(f == nullptr);
  ASSERT_EQ(LIBC_NAMESPACE::fgetspent_r(f, &entry, buf, sizeof(buf), &result),
            0);
  ASSERT_TRUE(result == &entry);
  ASSERT_STREQ(entry.sp_namp, "alice");
  ASSERT_EQ(LIBC_NAMESPACE::fclose(f), 0);
}

TEST_F(LlvmLibcShadowTest, BufferTooSmall) {
  struct spwd entry;
  char buf[4];
  struct spwd *result = nullptr;
  ASSERT_EQ(
      LIBC_NAMESPACE::getspnam_r("alice", &entry, buf, sizeof(buf), &result),
      ERANGE);
  ASSERT_TRUE(result == nullptr);
}

TEST_F(LlvmLibcShadowTest, WriteAnEntryOut) {
  auto out = libc_make_test_file_path("shadow_test.out");
  ::FILE *f = LIBC_NAMESPACE::fopen(out, "w");
  ASSERT_FALSE(f == nullptr);

  struct spwd *entry = LIBC_NAMESPACE::getspnam("alice");
  ASSERT_FALSE(entry == nullptr);
  ASSERT_EQ(LIBC_NAMESPACE::putspent(entry, f), 0);
  ASSERT_EQ(LIBC_NAMESPACE::fclose(f), 0);

  // What was written reads back as the same entry.
  ::FILE *r = LIBC_NAMESPACE::fopen(out, "r");
  ASSERT_FALSE(r == nullptr);
  struct spwd *read = LIBC_NAMESPACE::fgetspent(r);
  ASSERT_FALSE(read == nullptr);
  ASSERT_STREQ(read->sp_namp, "alice");
  ASSERT_EQ(read->sp_lstchg, 19500L);
  ASSERT_EQ(LIBC_NAMESPACE::fclose(r), 0);
  ASSERT_EQ(LIBC_NAMESPACE::remove(out), 0);
}

TEST_F(LlvmLibcShadowTest, LockAndUnlock) {
  // Taking the lock needs to write into /etc, which a test does not have.
  // Either answer is allowed; what matters is that unlocking after a failed
  // lock does not report success it did not have.
  int locked = LIBC_NAMESPACE::lckpwdf();
  if (locked == 0)
    ASSERT_EQ(LIBC_NAMESPACE::ulckpwdf(), 0);
  else
    ASSERT_EQ(locked, -1);
}
