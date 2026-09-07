//===-- Unittests for the Ethernet address conversions --------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/netinet/ether/ether_aton.h"
#include "src/netinet/ether/ether_aton_r.h"
#include "src/netinet/ether/ether_line.h"
#include "src/netinet/ether/ether_ntoa.h"
#include "src/netinet/ether/ether_ntoa_r.h"
#include "src/string/strcmp.h"
#include "test/UnitTest/Test.h"

namespace {

bool same(const struct ether_addr *addr, const unsigned char (&want)[6]) {
  for (int i = 0; i < 6; ++i)
    if (addr->ether_addr_octet[i] != want[i])
      return false;
  return true;
}

} // namespace

TEST(LlvmLibcEtherTest, ReadsAnAddress) {
  struct ether_addr addr;
  const unsigned char want[6] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};
  ASSERT_TRUE(LIBC_NAMESPACE::ether_aton_r("00:11:22:33:44:55", &addr) ==
              &addr);
  ASSERT_TRUE(same(&addr, want));
}

TEST(LlvmLibcEtherTest, ReadsUpperCaseAndSingleDigits) {
  struct ether_addr addr;
  const unsigned char want[6] = {0xaa, 0xbb, 0x00, 0x01, 0x02, 0x03};
  ASSERT_TRUE(LIBC_NAMESPACE::ether_aton_r("AA:bB:0:1:2:3", &addr) == &addr);
  ASSERT_TRUE(same(&addr, want));
}

// Everything after the sixth part is left alone, which is what lets a whole
// line of the ethers database be handed over.
TEST(LlvmLibcEtherTest, IgnoresWhatFollowsTheAddress) {
  struct ether_addr addr;
  const unsigned char want[6] = {0x08, 0x00, 0x20, 0x00, 0x61, 0xca};
  ASSERT_TRUE(LIBC_NAMESPACE::ether_aton_r("08:00:20:00:61:CA hoot", &addr) ==
              &addr);
  ASSERT_TRUE(same(&addr, want));
}

TEST(LlvmLibcEtherTest, RefusesWhatIsNotAnAddress) {
  struct ether_addr addr;
  ASSERT_TRUE(LIBC_NAMESPACE::ether_aton_r("", &addr) == nullptr);
  ASSERT_TRUE(LIBC_NAMESPACE::ether_aton_r("1:2:3:4:5", &addr) == nullptr);
  ASSERT_TRUE(LIBC_NAMESPACE::ether_aton_r("00-11-22-33-44-55", &addr) ==
              nullptr);
  ASSERT_TRUE(LIBC_NAMESPACE::ether_aton_r("00:11:22:33:44:5g", &addr) ==
              nullptr);
  ASSERT_TRUE(LIBC_NAMESPACE::ether_aton_r("100:11:22:33:44:55", &addr) ==
              nullptr);
}

// A part below sixteen is written with one digit, so what comes back is not
// always what went in.
TEST(LlvmLibcEtherTest, WritesAnAddress) {
  struct ether_addr addr;
  char buf[18];
  ASSERT_TRUE(LIBC_NAMESPACE::ether_aton_r("08:00:20:00:61:ca", &addr) !=
              nullptr);
  ASSERT_TRUE(LIBC_NAMESPACE::ether_ntoa_r(&addr, buf) == buf);
  ASSERT_STREQ(buf, "8:0:20:0:61:ca");
}

TEST(LlvmLibcEtherTest, TheAnswerSurvivesUntilTheNextCall) {
  struct ether_addr *first = LIBC_NAMESPACE::ether_aton("1:2:3:4:5:6");
  ASSERT_TRUE(first != nullptr);
  const unsigned char want[6] = {1, 2, 3, 4, 5, 6};
  ASSERT_TRUE(same(first, want));
  ASSERT_STREQ(LIBC_NAMESPACE::ether_ntoa(first), "1:2:3:4:5:6");
}

TEST(LlvmLibcEtherTest, ReadsALineOfTheDatabase) {
  struct ether_addr addr;
  char hostname[256];
  const unsigned char want[6] = {0x08, 0x00, 0x20, 0x00, 0x61, 0xca};

  ASSERT_EQ(LIBC_NAMESPACE::ether_line("08:00:20:00:61:CA  hoot # a comment",
                                       &addr, hostname),
            0);
  ASSERT_TRUE(same(&addr, want));
  ASSERT_STREQ(hostname, "hoot");

  // A line with no name on it, a line that is all comment and an empty line
  // each name nothing.
  ASSERT_EQ(LIBC_NAMESPACE::ether_line("08:00:20:00:61:CA", &addr, hostname),
            -1);
  ASSERT_EQ(LIBC_NAMESPACE::ether_line("# just a comment", &addr, hostname),
            -1);
  ASSERT_EQ(LIBC_NAMESPACE::ether_line("", &addr, hostname), -1);
}
