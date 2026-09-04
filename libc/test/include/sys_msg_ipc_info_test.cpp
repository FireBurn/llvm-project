//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the public sys/msg.h header.
///
//===----------------------------------------------------------------------===//

#include "include/sys/msg.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcSysMsgIpcInfoTest, MacrosAreDefined) {
#ifndef msg_cbytes
  FAIL() << "msg_cbytes is not defined";
#endif
}

TEST(LlvmLibcSysMsgIpcInfoTest, TypesAreComplete) {
  EXPECT_GT(sizeof(struct msginfo), size_t(0));
#if defined(__x86_64__) && defined(__linux__)
  EXPECT_EQ(sizeof(struct msginfo), size_t(32));
#endif
}
