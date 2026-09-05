//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for fgetpwent and putpwent.
///
//===----------------------------------------------------------------------===//

#include "hdr/fcntl_macros.h"
#include "hdr/sys_stat_macros.h"
#include "hdr/types/FILE.h"
#include "hdr/types/struct_passwd.h"
#include "src/__support/CPP/string_view.h"
#include "src/fcntl/open.h"
#include "src/pwd/fgetpwent.h"
#include "src/pwd/putpwent.h"
#include "src/stdio/fclose.h"
#include "src/stdio/fopen.h"
#include "src/stdio/remove.h"
#include "src/unistd/close.h"
#include "src/unistd/write.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/Test.h"

using LlvmLibcFGetPwEntTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;
using string_view = LIBC_NAMESPACE::cpp::string_view;

namespace {

constexpr const char *PASSWD_FILE = "fgetpwent.test.passwd";
constexpr const char *CONTENTS = "root:x:0:0:root:/root:/bin/sh\n"
                                 "andy:x:1000:1000::/home/andy:/bin/sh\n";

void write_passwd_file() {
  int fd =
      LIBC_NAMESPACE::open(PASSWD_FILE, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
  ASSERT_GT(fd, 0);
  string_view contents(CONTENTS);
  ASSERT_EQ(LIBC_NAMESPACE::write(fd, contents.data(), contents.size()),
            static_cast<ssize_t>(contents.size()));
  ASSERT_EQ(LIBC_NAMESPACE::close(fd), 0);
}

} // anonymous namespace

// fgetpwent reads from a stream the caller opened rather than from the
// password file the rest of the family reads.
TEST_F(LlvmLibcFGetPwEntTest, ReadsEveryEntryFromAStream) {
  write_passwd_file();

  ::FILE *f = LIBC_NAMESPACE::fopen(PASSWD_FILE, "r");
  ASSERT_TRUE(f != nullptr);

  struct passwd *p = LIBC_NAMESPACE::fgetpwent(f);
  ASSERT_TRUE(p != nullptr);
  EXPECT_STREQ(p->pw_name, "root");
  EXPECT_EQ(p->pw_uid, uid_t(0));
  EXPECT_STREQ(p->pw_dir, "/root");

  p = LIBC_NAMESPACE::fgetpwent(f);
  ASSERT_TRUE(p != nullptr);
  EXPECT_STREQ(p->pw_name, "andy");
  EXPECT_EQ(p->pw_uid, uid_t(1000));
  // A field the file left empty reads as empty rather than as missing.
  EXPECT_STREQ(p->pw_gecos, "");

  EXPECT_TRUE(LIBC_NAMESPACE::fgetpwent(f) == nullptr);

  ASSERT_EQ(LIBC_NAMESPACE::fclose(f), 0);
  ASSERT_EQ(LIBC_NAMESPACE::remove(PASSWD_FILE), 0);
}

// What putpwent writes reads back as the entry it was given.
TEST_F(LlvmLibcFGetPwEntTest, WritesAnEntryOut) {
  constexpr const char *OUT_FILE = "putpwent.test.passwd";

  char name[] = "andy";
  char password[] = "x";
  char gecos[] = "Andy";
  char dir[] = "/home/andy";
  char shell[] = "/bin/sh";
  struct passwd p = {};
  p.pw_name = name;
  p.pw_passwd = password;
  p.pw_uid = 1000;
  p.pw_gid = 1000;
  p.pw_gecos = gecos;
  p.pw_dir = dir;
  p.pw_shell = shell;

  ::FILE *out = LIBC_NAMESPACE::fopen(OUT_FILE, "w");
  ASSERT_TRUE(out != nullptr);
  ASSERT_EQ(LIBC_NAMESPACE::putpwent(&p, out), 0);
  ASSERT_EQ(LIBC_NAMESPACE::fclose(out), 0);

  ::FILE *in = LIBC_NAMESPACE::fopen(OUT_FILE, "r");
  ASSERT_TRUE(in != nullptr);
  struct passwd *read = LIBC_NAMESPACE::fgetpwent(in);
  ASSERT_TRUE(read != nullptr);
  EXPECT_STREQ(read->pw_name, "andy");
  EXPECT_EQ(read->pw_uid, uid_t(1000));
  EXPECT_EQ(read->pw_gid, gid_t(1000));
  EXPECT_STREQ(read->pw_gecos, "Andy");
  EXPECT_STREQ(read->pw_dir, "/home/andy");
  EXPECT_STREQ(read->pw_shell, "/bin/sh");
  ASSERT_EQ(LIBC_NAMESPACE::fclose(in), 0);

  ASSERT_EQ(LIBC_NAMESPACE::remove(OUT_FILE), 0);
}
