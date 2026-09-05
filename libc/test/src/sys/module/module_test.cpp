//===-- Unittests for init_module and delete_module -----------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "src/__support/libc_errno.h"
#include "src/sys/module/delete_module.h"
#include "src/sys/module/init_module.h"
#include "src/unistd/geteuid.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/Test.h"

using LlvmLibcModuleTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;

// Loading a module into the kernel needs CAP_SYS_MODULE, which a test does
// not have, and doing it where the test does run as root would change the
// machine. Only the refusal is checked.
TEST_F(LlvmLibcModuleTest, RefusedWithoutThePrivilege) {
  if (LIBC_NAMESPACE::geteuid() == 0)
    return;

  char image[8] = {};
  ASSERT_EQ(LIBC_NAMESPACE::init_module(image, sizeof(image), ""), -1L);
  ASSERT_ERRNO_EQ(EPERM);
  LIBC_NAMESPACE::libc_errno = 0;

  ASSERT_EQ(LIBC_NAMESPACE::delete_module("llvm_libc_not_a_module", 0), -1L);
  ASSERT_ERRNO_EQ(EPERM);
  LIBC_NAMESPACE::libc_errno = 0;
}
