//===-- Unittests for getpwnam and getpwuid -------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/types/struct_passwd.h"
#include "src/__support/File/file.h"
#include "src/__support/libc_errno.h"
#include "src/pwd/getpwent.h"
#include "src/pwd/getpwnam.h"
#include "src/pwd/getpwuid.h"
#include "src/pwd/pwd_utils.h"
#include "src/pwd/setpwent.h"
#include "src/stdio/remove.h"
#include "src/string/string_utils.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/Test.h"

namespace {

class HermeticFile {
  char path[256];

public:
  HermeticFile(const char *file_path, const char *content) {
    LIBC_NAMESPACE::internal::strlcpy(path, file_path, sizeof(path));
    auto file_or = LIBC_NAMESPACE::openfile(path, "w");
    if (file_or.has_value()) {
      auto *f = file_or.value();
      f->write(content, LIBC_NAMESPACE::internal::string_length(content));
      f->close();
    }
  }
  ~HermeticFile() { LIBC_NAMESPACE::remove(path); }
  const char *get_path() const { return path; }
};

constexpr const char *CONTENT = "root:x:0:0:root:/root:/bin/bash\n"
                                "bin:x:1:1:bin:/bin:/sbin/nologin\n"
                                "nobody:x:65534:65534::/:/sbin/nologin\n";

class LlvmLibcGetpwnamTest : public LIBC_NAMESPACE::testing::ErrnoCheckingTest {
};

} // namespace

TEST_F(LlvmLibcGetpwnamTest, FindsByName) {
  HermeticFile file(libc_make_test_file_path("getpwnam.test"), CONTENT);
  LIBC_NAMESPACE::passwd::TESTONLY_set_passwd_path(file.get_path());

  struct passwd *pwd = LIBC_NAMESPACE::getpwnam("bin");
  ASSERT_TRUE(pwd != nullptr);
  EXPECT_STREQ(pwd->pw_name, "bin");
  EXPECT_EQ(pwd->pw_uid, 1u);
  EXPECT_STREQ(pwd->pw_shell, "/sbin/nologin");
}

TEST_F(LlvmLibcGetpwnamTest, FindsByUid) {
  HermeticFile file(libc_make_test_file_path("getpwuid.test"), CONTENT);
  LIBC_NAMESPACE::passwd::TESTONLY_set_passwd_path(file.get_path());

  struct passwd *pwd = LIBC_NAMESPACE::getpwuid(65534);
  ASSERT_TRUE(pwd != nullptr);
  EXPECT_STREQ(pwd->pw_name, "nobody");
}

TEST_F(LlvmLibcGetpwnamTest, MissingEntryIsNotAnError) {
  HermeticFile file(libc_make_test_file_path("getpwnam_missing.test"), CONTENT);
  LIBC_NAMESPACE::passwd::TESTONLY_set_passwd_path(file.get_path());

  // Not finding a user is a null return with errno untouched, which is what
  // separates it from a failure to read the database.
  EXPECT_TRUE(LIBC_NAMESPACE::getpwnam("nosuchuser") == nullptr);
  ASSERT_ERRNO_SUCCESS();
  EXPECT_TRUE(LIBC_NAMESPACE::getpwuid(4242) == nullptr);
  ASSERT_ERRNO_SUCCESS();
}

TEST_F(LlvmLibcGetpwnamTest, NullNameIsRejected) {
  HermeticFile file(libc_make_test_file_path("getpwnam_null.test"), CONTENT);
  LIBC_NAMESPACE::passwd::TESTONLY_set_passwd_path(file.get_path());

  EXPECT_TRUE(LIBC_NAMESPACE::getpwnam(nullptr) == nullptr);
  ASSERT_ERRNO_EQ(EINVAL);
  LIBC_NAMESPACE::libc_errno = 0;
}

TEST_F(LlvmLibcGetpwnamTest, LookupResetsAnIteration) {
  HermeticFile file(libc_make_test_file_path("getpwnam_iter.test"), CONTENT);
  LIBC_NAMESPACE::passwd::TESTONLY_set_passwd_path(file.get_path());

  // A lookup shares the one open handle, so it rewinds the database and an
  // interleaved getpwent walk starts over. POSIX leaves this unspecified, but
  // glibc keeps the two independent, so pin the behaviour rather than let it
  // drift silently.
  LIBC_NAMESPACE::setpwent();
  struct passwd *first = LIBC_NAMESPACE::getpwent();
  ASSERT_TRUE(first != nullptr);
  EXPECT_STREQ(first->pw_name, "root");

  ASSERT_TRUE(LIBC_NAMESPACE::getpwnam("nobody") != nullptr);

  struct passwd *second = LIBC_NAMESPACE::getpwent();
  ASSERT_TRUE(second != nullptr);
  EXPECT_STREQ(second->pw_name, "root");
}
