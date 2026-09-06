//===-- Unittests for the list of login shells ----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/fcntl_macros.h"
#include "hdr/sys_stat_macros.h"
#include "src/fcntl/open.h"
#include "src/stdio/remove.h"
#include "src/unistd/close.h"
#include "src/unistd/endusershell.h"
#include "src/unistd/getusershell.h"
#include "src/unistd/setusershell.h"
#include "src/unistd/usershell.h"
#include "src/unistd/write.h"
#include "test/UnitTest/Test.h"

namespace {

constexpr const char *TEST_FILE = "usershell.test.shells";

void write_file(const char *contents) {
  int fd =
      LIBC_NAMESPACE::open(TEST_FILE, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
  ASSERT_GT(fd, 0);
  size_t length = 0;
  while (contents[length] != '\0')
    ++length;
  ASSERT_EQ(LIBC_NAMESPACE::write(fd, contents, length),
            static_cast<ssize_t>(length));
  ASSERT_EQ(LIBC_NAMESPACE::close(fd), 0);
}

} // anonymous namespace

class LlvmLibcUsershellTest : public LIBC_NAMESPACE::testing::Test {
public:
  void TearDown() override {
    LIBC_NAMESPACE::endusershell();
    LIBC_NAMESPACE::remove(TEST_FILE);
  }
};

TEST_F(LlvmLibcUsershellTest, ReadsOneShellPerLine) {
  write_file("/bin/sh\n/bin/bash\n/usr/bin/zsh\n");
  LIBC_NAMESPACE::usershell::TESTONLY_set_shells_path(TEST_FILE);

  LIBC_NAMESPACE::setusershell();
  ASSERT_STREQ(LIBC_NAMESPACE::getusershell(), "/bin/sh");
  ASSERT_STREQ(LIBC_NAMESPACE::getusershell(), "/bin/bash");
  ASSERT_STREQ(LIBC_NAMESPACE::getusershell(), "/usr/bin/zsh");
  ASSERT_TRUE(LIBC_NAMESPACE::getusershell() == nullptr);
}

TEST_F(LlvmLibcUsershellTest, PassesOverBlankLinesAndComments) {
  write_file("# the shells\n\n   \n  /bin/sh  \n#/bin/hidden\n/bin/bash\n");
  LIBC_NAMESPACE::usershell::TESTONLY_set_shells_path(TEST_FILE);

  LIBC_NAMESPACE::setusershell();
  // The leading blanks are trimmed, and so is anything after the name.
  ASSERT_STREQ(LIBC_NAMESPACE::getusershell(), "/bin/sh");
  ASSERT_STREQ(LIBC_NAMESPACE::getusershell(), "/bin/bash");
  ASSERT_TRUE(LIBC_NAMESPACE::getusershell() == nullptr);
}

TEST_F(LlvmLibcUsershellTest, StartsAgainFromTheTop) {
  write_file("/bin/sh\n/bin/bash\n");
  LIBC_NAMESPACE::usershell::TESTONLY_set_shells_path(TEST_FILE);

  LIBC_NAMESPACE::setusershell();
  ASSERT_STREQ(LIBC_NAMESPACE::getusershell(), "/bin/sh");
  LIBC_NAMESPACE::setusershell();
  ASSERT_STREQ(LIBC_NAMESPACE::getusershell(), "/bin/sh");
}

TEST_F(LlvmLibcUsershellTest, FallsBackWhenThereIsNoFile) {
  LIBC_NAMESPACE::usershell::TESTONLY_set_shells_path(
      "usershell.test.does.not.exist");
  LIBC_NAMESPACE::setusershell();
  // What every other implementation answers with rather than saying there
  // are no shells at all.
  ASSERT_STREQ(LIBC_NAMESPACE::getusershell(), "/bin/sh");
  ASSERT_STREQ(LIBC_NAMESPACE::getusershell(), "/bin/csh");
  ASSERT_TRUE(LIBC_NAMESPACE::getusershell() == nullptr);
}
