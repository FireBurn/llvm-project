//===-- Unittests for capget and capset -----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "src/sys/capability/cap_types.h"
#include "src/sys/capability/capget.h"
#include "src/sys/capability/capset.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

using LlvmLibcCapabilityTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Fails;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Succeeds;

// Reading this process's own capabilities needs none of them.
TEST_F(LlvmLibcCapabilityTest, ReadsOurOwnCapabilities) {
  __user_cap_header_struct header = {};
  // The third version of the layout, which every kernel since 2.6.26 takes.
  header.version = 0x20080522;
  header.pid = 0;

  __user_cap_data_struct data[2] = {};
  ASSERT_THAT(LIBC_NAMESPACE::capget(&header, data), Succeeds(0));

  // Writing back exactly what was read changes nothing and is allowed.
  ASSERT_THAT(LIBC_NAMESPACE::capset(&header, data), Succeeds(0));
}

// A version the kernel does not know is refused, and it says which version
// it wanted by writing it into the header.
TEST_F(LlvmLibcCapabilityTest, AVersionTheKernelDoesNotKnow) {
  __user_cap_header_struct header = {};
  header.version = 0;
  header.pid = 0;

  __user_cap_data_struct data[2] = {};
  ASSERT_THAT(LIBC_NAMESPACE::capget(&header, data), Fails(EINVAL));
  ASSERT_NE(header.version, 0U);
}
