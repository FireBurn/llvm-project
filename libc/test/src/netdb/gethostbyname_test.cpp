//===-- Unittests for gethostbyname and getservbyname ---------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/netdb_macros.h"
#include "hdr/sys_socket_macros.h"
#include "hdr/types/struct_hostent.h"
#include "hdr/types/struct_servent.h"
#include "src/netdb/__h_errno_location.h"
#include "src/netdb/gethostbyname.h"
#include "src/netdb/gethostbyname2.h"
#include "src/netdb/getservbyname.h"
#include "src/string/strcmp.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcGethostbynameTest, AnAddressNeedsNoLookup) {
  struct hostent *host = LIBC_NAMESPACE::gethostbyname("127.0.0.1");
  ASSERT_TRUE(host != nullptr);
  EXPECT_EQ(host->h_addrtype, AF_INET);
  EXPECT_EQ(host->h_length, 4);
  ASSERT_TRUE(host->h_addr_list[0] != nullptr);
  EXPECT_EQ(host->h_addr_list[1], static_cast<char *>(nullptr));
  const auto *bytes =
      reinterpret_cast<const unsigned char *>(host->h_addr_list[0]);
  EXPECT_EQ(bytes[0], static_cast<unsigned char>(127));
  EXPECT_EQ(bytes[3], static_cast<unsigned char>(1));
  // The names it goes by, of which there are none to report.
  ASSERT_TRUE(host->h_aliases != nullptr);
  EXPECT_EQ(host->h_aliases[0], static_cast<char *>(nullptr));
}

TEST(LlvmLibcGethostbynameTest, TheOtherKindOfAddressIsAskedForByName) {
  struct hostent *host = LIBC_NAMESPACE::gethostbyname2("::1", AF_INET6);
  ASSERT_TRUE(host != nullptr);
  EXPECT_EQ(host->h_addrtype, AF_INET6);
  EXPECT_EQ(host->h_length, 16);
  const auto *bytes =
      reinterpret_cast<const unsigned char *>(host->h_addr_list[0]);
  for (size_t i = 0; i < 15; ++i)
    EXPECT_EQ(bytes[i], static_cast<unsigned char>(0));
  EXPECT_EQ(bytes[15], static_cast<unsigned char>(1));

  // Asking for the wrong kind finds nothing rather than the other kind.
  EXPECT_EQ(LIBC_NAMESPACE::gethostbyname2("::1", AF_INET),
            static_cast<struct hostent *>(nullptr));
}

TEST(LlvmLibcGethostbynameTest, ReportsWhatWentWrongThroughItsOwnErrorNumber) {
  EXPECT_EQ(LIBC_NAMESPACE::gethostbyname(nullptr),
            static_cast<struct hostent *>(nullptr));
  EXPECT_EQ(*LIBC_NAMESPACE::__h_errno_location(), HOST_NOT_FOUND);

  // These report only one kind of address at a time, since the caller is told
  // one length for all of them.
  EXPECT_EQ(LIBC_NAMESPACE::gethostbyname2("127.0.0.1", AF_UNSPEC),
            static_cast<struct hostent *>(nullptr));
  EXPECT_EQ(*LIBC_NAMESPACE::__h_errno_location(), NO_RECOVERY);
}

TEST(LlvmLibcGetservbynameTest, FindsAServiceInTheFile) {
  struct servent *service = LIBC_NAMESPACE::getservbyname("http", "tcp");
  ASSERT_TRUE(service != nullptr);
  EXPECT_EQ(LIBC_NAMESPACE::strcmp(service->s_name, "http"), 0);
  EXPECT_EQ(LIBC_NAMESPACE::strcmp(service->s_proto, "tcp"), 0);
  // The port is kept in the order the wire uses.
  EXPECT_EQ(service->s_port, 0x5000);
  ASSERT_TRUE(service->s_aliases != nullptr);
  EXPECT_EQ(service->s_aliases[0], static_cast<char *>(nullptr));
}

TEST(LlvmLibcGetservbynameTest, WithNoProtocolTheOneFoundIsReported) {
  struct servent *service = LIBC_NAMESPACE::getservbyname("http", nullptr);
  ASSERT_TRUE(service != nullptr);
  EXPECT_EQ(service->s_port, 0x5000);
  EXPECT_TRUE(service->s_proto != nullptr);
  EXPECT_TRUE(service->s_proto[0] != '\0');
}

TEST(LlvmLibcGetservbynameTest, FindsNothingForWhatIsNotThere) {
  EXPECT_EQ(LIBC_NAMESPACE::getservbyname("not-a-real-service", "tcp"),
            static_cast<struct servent *>(nullptr));
  EXPECT_EQ(LIBC_NAMESPACE::getservbyname(nullptr, "tcp"),
            static_cast<struct servent *>(nullptr));
  // The protocol has to match too.
  EXPECT_EQ(LIBC_NAMESPACE::getservbyname("http", "not-a-protocol"),
            static_cast<struct servent *>(nullptr));
}
