//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for __llvm_libc_set_auxv
///
//===----------------------------------------------------------------------===//

#include "hdr/sys_auxv_macros.h"
#include "src/sys/auxv/getauxval.h"
#include "src/sys/auxv/linux/set_auxv.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/Test.h"

using LlvmLibcSetAuxvTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;

// The shared library is handed the vector the program started with, and from
// then on getauxval reads that one. The vector replaced here is not put back,
// so this is the only test in its binary.
TEST_F(LlvmLibcSetAuxvTest, GetauxvalReadsTheVectorItWasGiven) {
  static const unsigned long vector[] = {AT_PAGESZ, 0x1234, AT_UID, 77,
                                         AT_NULL,   0};
  __llvm_libc_set_auxv(vector);
  ASSERT_EQ(LIBC_NAMESPACE::getauxval(AT_PAGESZ), 0x1234ul);
  ASSERT_EQ(LIBC_NAMESPACE::getauxval(AT_UID), 77ul);
  ASSERT_ERRNO_SUCCESS();
  // What is not in it is not found.
  ASSERT_EQ(LIBC_NAMESPACE::getauxval(AT_GID), 0ul);
  ASSERT_ERRNO_EQ(ENOENT);
}
