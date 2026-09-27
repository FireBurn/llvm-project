//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for stdlib.h bringing in sys/types.h
///
//===----------------------------------------------------------------------===//

#define _DEFAULT_SOURCE 1
#include "include/stdlib.h"
#include "test/UnitTest/Test.h"

#ifndef _LLVM_LIBC_SYS_TYPES_H
#error "<stdlib.h> does not bring in <sys/types.h> under _DEFAULT_SOURCE"
#endif

TEST(LlvmLibcStdlibSysTypesTest, BsdTypesAreVisible) {
  u_int32_t word = 1;
  EXPECT_EQ(sizeof(word), size_t(4));
}
