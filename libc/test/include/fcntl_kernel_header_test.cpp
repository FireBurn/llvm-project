//===-- Unittests for fcntl types next to the kernel header ---------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Programs such as Mesa include <linux/fcntl.h> before <fcntl.h>. The types
/// libc declares must then not clash with the ones the kernel header defines.
///
//===----------------------------------------------------------------------===//

#include "test/UnitTest/Test.h"

#include <linux/fcntl.h>

#include "include/llvm-libc-types/struct_f_owner_ex.h"
#include "include/llvm-libc-types/struct_flock.h"
#include "include/llvm-libc-types/struct_flock64.h"

TEST(LlvmLibcFcntlKernelHeaderTest, TypesAfterTheKernelHeaderHaveItsLayout) {
  struct flock lock = {};
  struct flock64 lock64 = {};
  struct f_owner_ex owner = {F_OWNER_PGRP, 1};
  lock.l_type = F_WRLCK;
  lock64.l_len = 1;
  ASSERT_EQ(int(F_OWNER_TID), 0);
  ASSERT_EQ(int(F_OWNER_PID), 1);
  ASSERT_EQ(int(owner.type), int(F_OWNER_PGRP));
  ASSERT_EQ(sizeof(owner), sizeof(int) + sizeof(int));
  ASSERT_EQ(int(lock.l_type), int(F_WRLCK));
  ASSERT_EQ(int(lock64.l_len), 1);
}
