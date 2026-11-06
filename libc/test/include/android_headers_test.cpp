//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests that any public header gives Android code what bionic's does.
///
//===----------------------------------------------------------------------===//

#ifndef __ANDROID__
#define __ANDROID__ 1
#endif

#include "test/UnitTest/Test.h"

#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>

TEST(LlvmLibcAndroidHeadersTest, StdioBringsTheBionicIncludes) {
  off64_t offset = 0;
  __u32 word = 1;
  EXPECT_EQ(sizeof(offset), size_t(8));
  EXPECT_EQ(sizeof(word), size_t(4));
  EXPECT_EQ(F_SETFD, 2);
  EXPECT_GE(__ANDROID_API__, __ANDROID_API_V__);
}

// Android code takes the interface request for an IPv6 address from the
// netinet headers.
TEST(LlvmLibcAndroidHeadersTest, Ipv6InterfaceRequestIsDeclared) {
  struct in6_ifreq request = {};
  struct in6_addr address = {};
  EXPECT_EQ(sizeof(address), size_t(16));
  EXPECT_EQ(sizeof(request.ifr6_prefixlen), size_t(4));
  EXPECT_EQ(sizeof(request.ifr6_addr), size_t(16));
}

// bionic's <unistd.h> declares both, and code written for it does not include
// <fcntl.h> or <sys/ioctl.h>.
TEST(LlvmLibcAndroidHeadersTest, UnistdDeclaresFcntlAndIoctl) {
  int (*fcntl_function)(int, int, ...) = fcntl;
  int (*ioctl_function)(int, unsigned long, ...) = ioctl;
  EXPECT_TRUE(fcntl_function != nullptr);
  EXPECT_TRUE(ioctl_function != nullptr);
}

static int formatted(int, const char *, ...) __printflike(2, 3);
static int scanned(const char *, const char *, ...) __scanflike(2, 3);
static size_t timed(char *, size_t, const char *) __strftimelike(3);

TEST(LlvmLibcAndroidHeadersTest, FormatAttributesAreDefined) {
  EXPECT_EQ(formatted(1, "%d", 2), 1);
  EXPECT_EQ(scanned("1", "%d", nullptr), 2);
  EXPECT_EQ(timed(nullptr, 0, "%Y"), size_t(3));
}

static int formatted(int, const char *, ...) { return 1; }
static int scanned(const char *, const char *, ...) { return 2; }
static size_t timed(char *, size_t, const char *) { return 3; }

TEST(LlvmLibcAndroidHeadersTest, KernelTimespecIsDeclared) {
  struct __kernel_timespec ts = {};
  EXPECT_EQ(sizeof(ts), size_t(16));
}
