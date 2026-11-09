//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Integration test for the memory tagging setup done at startup.
///
//===----------------------------------------------------------------------===//

#include "hdr/sys_auxv_macros.h"
#include "src/__support/OSUtil/syscall.h"
#include "src/sys/auxv/getauxval.h"
#include "test/IntegrationTest/test.h"

#include <linux/prctl.h>
#include <sys/syscall.h>

// Where the CPU has MTE, the startup code enables the tagged address ABI and
// lets IRG generate every tag but 0, without turning tag checks on.
TEST_MAIN(int, char **, char **) {
  if (!(LIBC_NAMESPACE::getauxval(AT_HWCAP2) & HWCAP2_MTE))
    return 0;

  long ctrl =
      LIBC_NAMESPACE::syscall_impl<long>(SYS_prctl, PR_GET_TAGGED_ADDR_CTRL, 0,
                                         0, 0, 0);
  ASSERT_TRUE(ctrl >= 0);
  ASSERT_TRUE(ctrl & PR_TAGGED_ADDR_ENABLE);
  ASSERT_TRUE(((ctrl & PR_MTE_TAG_MASK) >> PR_MTE_TAG_SHIFT) == 0xfffe);
  ASSERT_TRUE((ctrl & PR_MTE_TCF_MASK) == PR_MTE_TCF_NONE);
  return 0;
}
