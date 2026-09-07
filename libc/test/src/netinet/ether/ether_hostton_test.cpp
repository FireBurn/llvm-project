//===-- Unittests for ether_hostton and ether_ntohost ---------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/fcntl_macros.h"
#include "hdr/sys_stat_macros.h"
#include "hdr/types/struct_ether_addr.h"
#include "src/__support/CPP/string_view.h"
#include "src/fcntl/open.h"
#include "src/netinet/ether/ether_hostton.h"
#include "src/netinet/ether/ether_ntohost.h"
#include "src/netinet/ether/ethers.h"
#include "src/stdio/remove.h"
#include "src/unistd/close.h"
#include "src/unistd/write.h"
#include "test/UnitTest/Test.h"

using string_view = LIBC_NAMESPACE::cpp::string_view;

namespace {

constexpr const char *ETHERS_FILE = "ether_hostton.test.ethers";
constexpr const char *CONTENTS = "# a comment, and then a blank line\n"
                                 "\n"
                                 "00:11:22:33:44:55 first\n"
                                 "aa:bb:cc:dd:ee:ff second\n";

class LlvmLibcEtherHosttonTest : public LIBC_NAMESPACE::testing::Test {
public:
  void SetUp() override {
    int fd = LIBC_NAMESPACE::open(ETHERS_FILE, O_WRONLY | O_CREAT | O_TRUNC,
                                  S_IRWXU);
    ASSERT_GT(fd, 0);
    string_view contents(CONTENTS);
    ASSERT_EQ(LIBC_NAMESPACE::write(fd, contents.data(), contents.size()),
              static_cast<ssize_t>(contents.size()));
    ASSERT_EQ(LIBC_NAMESPACE::close(fd), 0);
    LIBC_NAMESPACE::ether::TESTONLY_set_ethers_path(ETHERS_FILE);
  }

  void TearDown() override {
    LIBC_NAMESPACE::ether::TESTONLY_set_ethers_path(nullptr);
    LIBC_NAMESPACE::remove(ETHERS_FILE);
  }
};

} // anonymous namespace

TEST_F(LlvmLibcEtherHosttonTest, FindsTheAddressOfAName) {
  struct ether_addr addr = {};
  ASSERT_EQ(LIBC_NAMESPACE::ether_hostton("second", &addr), 0);
  EXPECT_EQ(addr.ether_addr_octet[0], uint8_t(0xaa));
  EXPECT_EQ(addr.ether_addr_octet[1], uint8_t(0xbb));
  EXPECT_EQ(addr.ether_addr_octet[2], uint8_t(0xcc));
  EXPECT_EQ(addr.ether_addr_octet[3], uint8_t(0xdd));
  EXPECT_EQ(addr.ether_addr_octet[4], uint8_t(0xee));
  EXPECT_EQ(addr.ether_addr_octet[5], uint8_t(0xff));
}

TEST_F(LlvmLibcEtherHosttonTest, FindsTheNameOfAnAddress) {
  struct ether_addr addr = {};
  addr.ether_addr_octet[0] = 0x00;
  addr.ether_addr_octet[1] = 0x11;
  addr.ether_addr_octet[2] = 0x22;
  addr.ether_addr_octet[3] = 0x33;
  addr.ether_addr_octet[4] = 0x44;
  addr.ether_addr_octet[5] = 0x55;

  char name[256] = {};
  ASSERT_EQ(LIBC_NAMESPACE::ether_ntohost(name, &addr), 0);
  EXPECT_STREQ(name, "first");
}

TEST_F(LlvmLibcEtherHosttonTest, NothingThatIsNotThere) {
  struct ether_addr addr = {};
  EXPECT_EQ(LIBC_NAMESPACE::ether_hostton("absent", &addr), -1);

  struct ether_addr unknown = {};
  unknown.ether_addr_octet[0] = 0xde;
  char name[256] = {};
  EXPECT_EQ(LIBC_NAMESPACE::ether_ntohost(name, &unknown), -1);
}

TEST_F(LlvmLibcEtherHosttonTest, NullArgumentsAreRefused) {
  struct ether_addr addr = {};
  char name[256] = {};
  EXPECT_EQ(LIBC_NAMESPACE::ether_hostton(nullptr, &addr), -1);
  EXPECT_EQ(LIBC_NAMESPACE::ether_hostton("first", nullptr), -1);
  EXPECT_EQ(LIBC_NAMESPACE::ether_ntohost(nullptr, &addr), -1);
  EXPECT_EQ(LIBC_NAMESPACE::ether_ntohost(name, nullptr), -1);
}
