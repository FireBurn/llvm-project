//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for android_getaddrinfofornet.
///
//===----------------------------------------------------------------------===//

#include "hdr/sys_socket_macros.h"
#include "hdr/types/struct_addrinfo.h"
#include "hdr/types/struct_sockaddr_in.h"
#include "src/android/android_getaddrinfofornet.h"
#include "src/netdb/freeaddrinfo.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/Test.h"

using LlvmLibcAndroidGetaddrinfofornetTest =
    LIBC_NAMESPACE::testing::ErrnoCheckingTest;

// The network id and the mark select nothing: an address is converted the way
// getaddrinfo converts it, whatever they are.
TEST_F(LlvmLibcAndroidGetaddrinfofornetTest, AnAddressNeedsNoLookup) {
  struct addrinfo hints = {};
  hints.ai_socktype = SOCK_STREAM;
  struct addrinfo *res = nullptr;
  ASSERT_EQ(LIBC_NAMESPACE::android_getaddrinfofornet("127.0.0.1", nullptr,
                                                      &hints, 7, 0x10, &res),
            0);
  ASSERT_TRUE(res != nullptr);
  EXPECT_EQ(res->ai_family, AF_INET);
  const auto *address = reinterpret_cast<struct sockaddr_in *>(res->ai_addr);
  const auto *bytes =
      reinterpret_cast<const unsigned char *>(&address->sin_addr);
  EXPECT_EQ(bytes[0], static_cast<unsigned char>(127));
  EXPECT_EQ(bytes[3], static_cast<unsigned char>(1));
  LIBC_NAMESPACE::freeaddrinfo(res);
}
