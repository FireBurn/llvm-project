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

TEST(LlvmLibcGetNameInfoTest, NameRequiredCannotBeMet) {
  struct sockaddr_in addr{};
  addr.sin_family = AF_INET;
  unsigned char octets[4] = {127, 0, 0, 1};
  __builtin_memcpy(&addr.sin_addr, octets, 4);

  char host[64] = {};
  // There is no resolver, so a caller which will not take the numeric form
  // is told the name is not there.
  EXPECT_EQ(LIBC_NAMESPACE::getnameinfo(
                reinterpret_cast<struct sockaddr *>(&addr), sizeof(addr), host,
                sizeof(host), nullptr, 0, NI_NAMEREQD),
            EAI_NONAME);
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
