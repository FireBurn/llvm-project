//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for getservbyname_r and getservbyport_r
///
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/types/struct_servent.h"
#include "src/netdb/getservbyname_r.h"
#include "src/netdb/getservbyport_r.h"
#include "test/UnitTest/Test.h"

// What is read comes from /etc/services, so these say what has to hold of
// whatever it names rather than assuming any particular service is there.

TEST(LlvmLibcGetServByNameRTest, NoNameIsNoAnswer) {
  struct servent entry;
  struct servent *result = &entry;
  char buffer[512];
  EXPECT_EQ(LIBC_NAMESPACE::getservbyname_r("no such service", nullptr, &entry,
                                            buffer, sizeof(buffer), &result),
            0);
  EXPECT_TRUE(result == nullptr);
}

TEST(LlvmLibcGetServByNameRTest, NameAndPortAgree) {
  struct servent by_name;
  struct servent *found = nullptr;
  char buffer[512];
  if (LIBC_NAMESPACE::getservbyname_r("ssh", "tcp", &by_name, buffer,
                                      sizeof(buffer), &found) != 0 ||
      found == nullptr)
    return;
  ASSERT_STREQ(by_name.s_name, "ssh");
  ASSERT_STREQ(by_name.s_proto, "tcp");

  struct servent by_port;
  struct servent *again = nullptr;
  char other[512];
  ASSERT_EQ(LIBC_NAMESPACE::getservbyport_r(by_name.s_port, "tcp", &by_port,
                                            other, sizeof(other), &again),
            0);
  ASSERT_TRUE(again == &by_port);
  ASSERT_EQ(by_port.s_port, by_name.s_port);
  ASSERT_STREQ(by_port.s_name, "ssh");
}

// A buffer too small for the entry is reported, not overrun.
TEST(LlvmLibcGetServByNameRTest, ABufferThatIsTooSmall) {
  struct servent entry;
  struct servent *result = &entry;
  char buffer[512];
  if (LIBC_NAMESPACE::getservbyname_r("ssh", "tcp", &entry, buffer,
                                      sizeof(buffer), &result) != 0 ||
      result == nullptr)
    return;
  char tiny[2];
  struct servent small;
  struct servent *none = &small;
  EXPECT_EQ(LIBC_NAMESPACE::getservbyname_r("ssh", "tcp", &small, tiny,
                                            sizeof(tiny), &none),
            ERANGE);
  EXPECT_TRUE(none == nullptr);
}
