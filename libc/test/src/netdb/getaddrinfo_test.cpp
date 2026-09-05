//===-- Unittests for getaddrinfo -----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/netdb_macros.h"
#include "hdr/sys_socket_macros.h"
#include "hdr/types/struct_addrinfo.h"
#include "hdr/types/struct_sockaddr_in.h"
#include "hdr/types/struct_sockaddr_in6.h"
#include "src/netdb/freeaddrinfo.h"
#include "src/netdb/getaddrinfo.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/Test.h"

using LlvmLibcNetdbTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;

namespace {

// How many records a list holds.
size_t length_of(const struct addrinfo *list) {
  size_t count = 0;
  for (const struct addrinfo *entry = list; entry != nullptr;
       entry = entry->ai_next)
    ++count;
  return count;
}

} // anonymous namespace

TEST_F(LlvmLibcNetdbTest, AnAddressNeedsNoLookup) {
  struct addrinfo hints = {};
  hints.ai_socktype = SOCK_STREAM;
  struct addrinfo *res = nullptr;
  ASSERT_EQ(LIBC_NAMESPACE::getaddrinfo("127.0.0.1", nullptr, &hints, &res), 0);
  ASSERT_TRUE(res != nullptr);
  EXPECT_EQ(res->ai_family, AF_INET);
  EXPECT_EQ(res->ai_socktype, SOCK_STREAM);
  EXPECT_EQ(res->ai_addrlen,
            static_cast<socklen_t>(sizeof(struct sockaddr_in)));
  const auto *address = reinterpret_cast<struct sockaddr_in *>(res->ai_addr);
  EXPECT_EQ(address->sin_family, static_cast<sa_family_t>(AF_INET));
  const auto *bytes =
      reinterpret_cast<const unsigned char *>(&address->sin_addr);
  EXPECT_EQ(bytes[0], static_cast<unsigned char>(127));
  EXPECT_EQ(bytes[3], static_cast<unsigned char>(1));
  LIBC_NAMESPACE::freeaddrinfo(res);
}

TEST_F(LlvmLibcNetdbTest, AVersionSixAddressNeedsNoLookupEither) {
  struct addrinfo hints = {};
  hints.ai_socktype = SOCK_STREAM;
  struct addrinfo *res = nullptr;
  ASSERT_EQ(LIBC_NAMESPACE::getaddrinfo("::1", nullptr, &hints, &res), 0);
  ASSERT_TRUE(res != nullptr);
  EXPECT_EQ(res->ai_family, AF_INET6);
  EXPECT_EQ(res->ai_addrlen,
            static_cast<socklen_t>(sizeof(struct sockaddr_in6)));
  const auto *address = reinterpret_cast<struct sockaddr_in6 *>(res->ai_addr);
  const auto *bytes =
      reinterpret_cast<const unsigned char *>(&address->sin6_addr);
  for (size_t i = 0; i < 15; ++i)
    EXPECT_EQ(bytes[i], static_cast<unsigned char>(0));
  EXPECT_EQ(bytes[15], static_cast<unsigned char>(1));
  LIBC_NAMESPACE::freeaddrinfo(res);
}

TEST_F(LlvmLibcNetdbTest, ServiceGivenAsANumberIsThatNumber) {
  struct addrinfo hints = {};
  hints.ai_socktype = SOCK_STREAM;
  struct addrinfo *res = nullptr;
  ASSERT_EQ(LIBC_NAMESPACE::getaddrinfo("127.0.0.1", "8080", &hints, &res), 0);
  ASSERT_TRUE(res != nullptr);
  const auto *address = reinterpret_cast<struct sockaddr_in *>(res->ai_addr);
  // The port is kept in the order the wire uses.
  EXPECT_EQ(address->sin_port, static_cast<uint16_t>(0x901F));
  LIBC_NAMESPACE::freeaddrinfo(res);
}

TEST_F(LlvmLibcNetdbTest, NoNodeAndNoServiceIsNothingToLookUp) {
  struct addrinfo *res = nullptr;
  EXPECT_EQ(LIBC_NAMESPACE::getaddrinfo(nullptr, nullptr, nullptr, &res),
            EAI_NONAME);
}

TEST_F(LlvmLibcNetdbTest, NoNodeMeansThisMachine) {
  struct addrinfo hints = {};
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_family = AF_INET;
  struct addrinfo *res = nullptr;

  // Somewhere to connect back to here.
  ASSERT_EQ(LIBC_NAMESPACE::getaddrinfo(nullptr, "80", &hints, &res), 0);
  ASSERT_TRUE(res != nullptr);
  const auto *loopback = reinterpret_cast<struct sockaddr_in *>(res->ai_addr);
  const auto *bytes =
      reinterpret_cast<const unsigned char *>(&loopback->sin_addr);
  EXPECT_EQ(bytes[0], static_cast<unsigned char>(127));
  LIBC_NAMESPACE::freeaddrinfo(res);

  // And somewhere to listen, which is any address this machine has.
  hints.ai_flags = AI_PASSIVE;
  res = nullptr;
  ASSERT_EQ(LIBC_NAMESPACE::getaddrinfo(nullptr, "80", &hints, &res), 0);
  ASSERT_TRUE(res != nullptr);
  const auto *any = reinterpret_cast<struct sockaddr_in *>(res->ai_addr);
  const auto *any_bytes =
      reinterpret_cast<const unsigned char *>(&any->sin_addr);
  for (size_t i = 0; i < 4; ++i)
    EXPECT_EQ(any_bytes[i], static_cast<unsigned char>(0));
  LIBC_NAMESPACE::freeaddrinfo(res);
}

TEST_F(LlvmLibcNetdbTest, NumericHostMeansNothingIsLookedUp) {
  struct addrinfo hints = {};
  hints.ai_flags = AI_NUMERICHOST;
  struct addrinfo *res = nullptr;
  EXPECT_EQ(LIBC_NAMESPACE::getaddrinfo("a.name.that.is.not.an.address",
                                        nullptr, &hints, &res),
            EAI_NONAME);
  EXPECT_EQ(res, static_cast<struct addrinfo *>(nullptr));
}

TEST_F(LlvmLibcNetdbTest, RejectsWhatItCannotAnswer) {
  struct addrinfo hints = {};
  struct addrinfo *res = nullptr;

  hints.ai_family = 1234;
  EXPECT_EQ(LIBC_NAMESPACE::getaddrinfo("127.0.0.1", nullptr, &hints, &res),
            EAI_FAMILY);

  hints = {};
  hints.ai_socktype = 1234;
  EXPECT_EQ(LIBC_NAMESPACE::getaddrinfo("127.0.0.1", nullptr, &hints, &res),
            EAI_SOCKTYPE);

  hints = {};
  hints.ai_flags = AI_NUMERICSERV;
  EXPECT_EQ(
      LIBC_NAMESPACE::getaddrinfo("127.0.0.1", "notanumber", &hints, &res),
      EAI_NONAME);

  hints = {};
  EXPECT_EQ(LIBC_NAMESPACE::getaddrinfo("127.0.0.1", "99999", &hints, &res),
            EAI_SERVICE);
}

TEST_F(LlvmLibcNetdbTest, EveryKindOfSocketIsReportedWhenNoneIsAskedFor) {
  struct addrinfo *res = nullptr;
  ASSERT_EQ(LIBC_NAMESPACE::getaddrinfo("127.0.0.1", nullptr, nullptr, &res),
            0);
  // One address, reported for each kind of socket it can be used with.
  EXPECT_EQ(length_of(res), size_t(3));
  LIBC_NAMESPACE::freeaddrinfo(res);

  struct addrinfo hints = {};
  hints.ai_socktype = SOCK_DGRAM;
  res = nullptr;
  ASSERT_EQ(LIBC_NAMESPACE::getaddrinfo("127.0.0.1", nullptr, &hints, &res), 0);
  EXPECT_EQ(length_of(res), size_t(1));
  EXPECT_EQ(res->ai_socktype, SOCK_DGRAM);
  LIBC_NAMESPACE::freeaddrinfo(res);
}

TEST_F(LlvmLibcNetdbTest, FreeingNothingIsFine) {
  LIBC_NAMESPACE::freeaddrinfo(nullptr);
}
