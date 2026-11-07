//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Tests that struct iovec can come from both sys/uio.h and linux/uio.h.
///
//===----------------------------------------------------------------------===//

#ifndef __ANDROID__
#define __ANDROID__ 1
#endif

#include "test/UnitTest/Test.h"

// The kernel's header first, then ours: neither may define the structure
// again.
#include <linux/uio.h>

#include "include/llvm-libc-types/struct_iovec.h"

TEST(LlvmLibcAndroidIovecTest, KernelAndLibcHeadersAgree) {
  struct iovec vec = {nullptr, 7};
  EXPECT_EQ(vec.iov_len, size_t(7));
  EXPECT_EQ(sizeof(vec), 2 * sizeof(void *));
}
