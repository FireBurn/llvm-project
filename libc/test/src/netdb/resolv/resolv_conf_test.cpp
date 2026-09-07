//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Tests for reading resolv.conf.
///
//===----------------------------------------------------------------------===//

#include "hdr/sys_socket_macros.h"
#include "src/__support/CPP/string_view.h"
#include "src/netdb/resolv/resolv_conf.h"
#include "src/string/strcmp.h"
#include "test/UnitTest/Test.h"

using LIBC_NAMESPACE::cpp::string_view;
using LIBC_NAMESPACE::resolv::MAX_SEARCH;
using LIBC_NAMESPACE::resolv::ResolvConf;

TEST(LlvmLibcResolvConfTest, NamesNobodyAsksTheMachineItself) {
  ResolvConf conf;
  ASSERT_TRUE(conf.parse(string_view("")));
  ASSERT_EQ(conf.server_count, size_t(1));
  ASSERT_EQ(conf.servers[0].family, AF_INET);
  ASSERT_EQ(conf.servers[0].bytes[0], static_cast<unsigned char>(127));
  ASSERT_EQ(conf.servers[0].bytes[3], static_cast<unsigned char>(1));
  ASSERT_EQ(conf.search_count, size_t(0));
  ASSERT_EQ(conf.ndots, size_t(1));
  ASSERT_EQ(conf.timeout_seconds, size_t(5));
  ASSERT_EQ(conf.attempts, size_t(2));
}

TEST(LlvmLibcResolvConfTest, ReadsServersInOrderAndIgnoresComments) {
  ResolvConf conf;
  ASSERT_TRUE(conf.parse(string_view("# a comment\n"
                                     "nameserver 192.0.2.1 ; another\n"
                                     "nameserver 2001:db8::1\n"
                                     "nameserver not-an-address\n")));
  ASSERT_EQ(conf.server_count, size_t(2));
  ASSERT_EQ(conf.servers[0].family, AF_INET);
  ASSERT_EQ(conf.servers[0].bytes[0], static_cast<unsigned char>(192));
  ASSERT_EQ(conf.servers[1].family, AF_INET6);
  ASSERT_EQ(conf.servers[1].bytes[0], static_cast<unsigned char>(0x20));
  ASSERT_EQ(conf.servers[1].scope, 0u);
}

TEST(LlvmLibcResolvConfTest, KeepsOnlyAsManyServersAsAreWorthKeeping) {
  ResolvConf conf;
  ASSERT_TRUE(conf.parse(string_view("nameserver 192.0.2.1\n"
                                     "nameserver 192.0.2.2\n"
                                     "nameserver 192.0.2.3\n"
                                     "nameserver 192.0.2.4\n")));
  ASSERT_EQ(conf.server_count, LIBC_NAMESPACE::resolv::MAX_SERVERS);
  ASSERT_EQ(conf.servers[2].bytes[3], static_cast<unsigned char>(3));
}

TEST(LlvmLibcResolvConfTest, SearchListTakesEveryNameAndTheLastLineWins) {
  ResolvConf conf;
  ASSERT_TRUE(conf.parse(string_view("search a.example b.example\n"
                                     "search c.example d.example e.example\n")));
  ASSERT_EQ(conf.search_count, size_t(3));
  ASSERT_EQ(LIBC_NAMESPACE::strcmp(conf.search[0], "c.example"), 0);
  ASSERT_EQ(LIBC_NAMESPACE::strcmp(conf.search[2], "e.example"), 0);
}

TEST(LlvmLibcResolvConfTest, DomainNamesOneDomainAndLaterLinesReplaceIt) {
  ResolvConf conf;
  ASSERT_TRUE(conf.parse(string_view("domain a.example b.example\n")));
  ASSERT_EQ(conf.search_count, size_t(1));
  ASSERT_EQ(LIBC_NAMESPACE::strcmp(conf.search[0], "a.example"), 0);

  ASSERT_TRUE(conf.parse(string_view("search c.example\n")));
  ASSERT_EQ(conf.search_count, size_t(1));
  ASSERT_EQ(LIBC_NAMESPACE::strcmp(conf.search[0], "c.example"), 0);
}

TEST(LlvmLibcResolvConfTest, SearchListStopsAtTheLimit) {
  ResolvConf conf;
  ASSERT_TRUE(conf.parse(string_view("search a b c d e f g h\n")));
  ASSERT_EQ(conf.search_count, MAX_SEARCH);
}

TEST(LlvmLibcResolvConfTest, OptionsSetNdotsTimeoutAndAttempts) {
  ResolvConf conf;
  ASSERT_TRUE(
      conf.parse(string_view("options ndots:3 timeout:2 attempts:4 rotate\n")));
  ASSERT_EQ(conf.ndots, size_t(3));
  ASSERT_EQ(conf.timeout_seconds, size_t(2));
  ASSERT_EQ(conf.attempts, size_t(4));
}

TEST(LlvmLibcResolvConfTest, OptionsKeepTheirLimitsAndRefuseNonsense) {
  ResolvConf conf;
  // ndots is capped, and a timeout or attempt count of zero would mean never
  // getting an answer, so those stay as they were. Zero dots is allowed.
  ASSERT_TRUE(conf.parse(
      string_view("options ndots:99 timeout:0 attempts:0 timeout:x\n")));
  ASSERT_EQ(conf.ndots, size_t(15));
  ASSERT_EQ(conf.timeout_seconds, size_t(5));
  ASSERT_EQ(conf.attempts, size_t(2));

  ResolvConf zero;
  ASSERT_TRUE(zero.parse(string_view("options ndots:0\n")));
  ASSERT_EQ(zero.ndots, size_t(0));
}
