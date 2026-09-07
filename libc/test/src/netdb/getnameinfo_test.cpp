//===-- Unittests for getnameinfo -----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/netdb_macros.h"
#include "hdr/types/struct_sockaddr.h"
#include "hdr/types/struct_sockaddr_in.h"
#include "hdr/types/struct_sockaddr_in6.h"
#include "src/__support/CPP/string_view.h"
#include "src/arpa/inet/htons.h"
#include "src/netdb/getnameinfo.h"
#include "test/UnitTest/Test.h"

#include <sys/socket.h>

using string_view = LIBC_NAMESPACE::cpp::string_view;

TEST(LlvmLibcGetNameInfoTest, IPv4AddressAndPort) {
  struct sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_port = LIBC_NAMESPACE::htons(8080);
  // 127.0.0.1 in network order.
  unsigned char octets[4] = {127, 0, 0, 1};
  __builtin_memcpy(&addr.sin_addr, octets, 4);

  char host[64] = {};
  char serv[16] = {};
  ASSERT_EQ(LIBC_NAMESPACE::getnameinfo(
                reinterpret_cast<struct sockaddr *>(&addr), sizeof(addr), host,
                sizeof(host), serv, sizeof(serv), NI_NUMERICHOST),
            0);
  EXPECT_TRUE(string_view(host) == "127.0.0.1");
  EXPECT_TRUE(string_view(serv) == "8080");
}

TEST(LlvmLibcGetNameInfoTest, IPv6Address) {
  struct sockaddr_in6 addr{};
  addr.sin6_family = AF_INET6;
  addr.sin6_port = LIBC_NAMESPACE::htons(443);
  // ::1
  unsigned char bytes[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1};
  __builtin_memcpy(&addr.sin6_addr, bytes, 16);

  char host[64] = {};
  char serv[16] = {};
  ASSERT_EQ(LIBC_NAMESPACE::getnameinfo(
                reinterpret_cast<struct sockaddr *>(&addr), sizeof(addr), host,
                sizeof(host), serv, sizeof(serv), NI_NUMERICHOST),
            0);
  EXPECT_TRUE(string_view(host) == "::1");
  EXPECT_TRUE(string_view(serv) == "443");
}

TEST(LlvmLibcGetNameInfoTest, HostOrServiceAlone) {
  struct sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_port = LIBC_NAMESPACE::htons(22);
  unsigned char octets[4] = {10, 0, 0, 5};
  __builtin_memcpy(&addr.sin_addr, octets, 4);

  char host[64] = {};
  ASSERT_EQ(LIBC_NAMESPACE::getnameinfo(
                reinterpret_cast<struct sockaddr *>(&addr), sizeof(addr), host,
                sizeof(host), nullptr, 0, NI_NUMERICHOST),
            0);
  EXPECT_TRUE(string_view(host) == "10.0.0.5");

  char serv[16] = {};
  ASSERT_EQ(LIBC_NAMESPACE::getnameinfo(
                reinterpret_cast<struct sockaddr *>(&addr), sizeof(addr),
                nullptr, 0, serv, sizeof(serv), NI_NUMERICSERV),
            0);
  EXPECT_TRUE(string_view(serv) == "22");
}

// Asking for a name gives one where the address has one, and says there is
// none where it has not. Which of the two happens depends on what the system
// this runs on says about the address, so both are accepted; what is checked
// is that the two agree with each other.
TEST(LlvmLibcGetNameInfoTest, NameRequiredIsMetOrRefused) {
  struct sockaddr_in addr{};
  addr.sin_family = AF_INET;
  unsigned char octets[4] = {127, 0, 0, 1};
  __builtin_memcpy(&addr.sin_addr, octets, 4);

  char host[256] = {};
  const int result = LIBC_NAMESPACE::getnameinfo(
      reinterpret_cast<struct sockaddr *>(&addr), sizeof(addr), host,
      sizeof(host), nullptr, 0, NI_NAMEREQD);
  if (result == 0) {
    // A name was found, so it is a name and not the address written back.
    ASSERT_NE(host[0], '\0');
    ASSERT_STRNE(host, "127.0.0.1");
  } else {
    ASSERT_EQ(result, EAI_NONAME);
  }
}

// Asking for the numeric form gives it whatever the address is called.
TEST(LlvmLibcGetNameInfoTest, NumericHostIsAlwaysTheAddress) {
  struct sockaddr_in addr{};
  addr.sin_family = AF_INET;
  unsigned char octets[4] = {127, 0, 0, 1};
  __builtin_memcpy(&addr.sin_addr, octets, 4);

  char host[256] = {};
  ASSERT_EQ(LIBC_NAMESPACE::getnameinfo(
                reinterpret_cast<struct sockaddr *>(&addr), sizeof(addr), host,
                sizeof(host), nullptr, 0, NI_NUMERICHOST),
            0);
  ASSERT_STREQ(host, "127.0.0.1");
}

TEST(LlvmLibcGetNameInfoTest, BadArguments) {
  struct sockaddr_in addr{};
  addr.sin_family = AF_INET;
  char host[64] = {};

  EXPECT_EQ(LIBC_NAMESPACE::getnameinfo(nullptr, sizeof(addr), host,
                                        sizeof(host), nullptr, 0, 0),
            EAI_FAMILY);
  // Neither a host nor a service was asked for.
  EXPECT_EQ(
      LIBC_NAMESPACE::getnameinfo(reinterpret_cast<struct sockaddr *>(&addr),
                                  sizeof(addr), nullptr, 0, nullptr, 0, 0),
      EAI_NONAME);
  // A length too short for the family named.
  EXPECT_EQ(
      LIBC_NAMESPACE::getnameinfo(reinterpret_cast<struct sockaddr *>(&addr), 4,
                                  host, sizeof(host), nullptr, 0, 0),
      EAI_FAMILY);

  struct sockaddr other{};
  other.sa_family = AF_UNIX;
  EXPECT_EQ(LIBC_NAMESPACE::getnameinfo(&other, sizeof(other), host,
                                        sizeof(host), nullptr, 0, 0),
            EAI_FAMILY);
}

TEST(LlvmLibcGetNameInfoTest, BuffersTooSmall) {
  struct sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_port = LIBC_NAMESPACE::htons(65535);
  unsigned char octets[4] = {192, 168, 100, 200};
  __builtin_memcpy(&addr.sin_addr, octets, 4);

  char host[4] = {};
  EXPECT_EQ(LIBC_NAMESPACE::getnameinfo(
                reinterpret_cast<struct sockaddr *>(&addr), sizeof(addr), host,
                sizeof(host), nullptr, 0, NI_NUMERICHOST),
            EAI_OVERFLOW);

  char serv[3] = {};
  EXPECT_EQ(LIBC_NAMESPACE::getnameinfo(
                reinterpret_cast<struct sockaddr *>(&addr), sizeof(addr),
                nullptr, 0, serv, sizeof(serv), NI_NUMERICSERV),
            EAI_OVERFLOW);
}
