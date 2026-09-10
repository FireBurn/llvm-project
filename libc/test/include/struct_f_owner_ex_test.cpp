//===-- Unittests for struct f_owner_ex ---------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for struct f_owner_ex.
///
//===----------------------------------------------------------------------===//

#include "test/UnitTest/Test.h"

#include "include/llvm-libc-types/struct_f_owner_ex.h"

// A program may define its own pid_type, which is why the enum is not called
// that. Declaring one next to the struct must therefore compile.
namespace {
enum pid_type { MY_PID_TYPE };
} // namespace

TEST(LlvmLibcStructFOwnerExTest, KeepsTheEnumeratorNames) {
  struct f_owner_ex owner = {F_OWNER_PGRP, 1};
  ASSERT_EQ(int(F_OWNER_TID), 0);
  ASSERT_EQ(int(F_OWNER_PID), 1);
  ASSERT_EQ(int(owner.type), int(F_OWNER_PGRP));
  ASSERT_EQ(int(MY_PID_TYPE), 0);
}
