//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the public setjmp.h header.
///
//===----------------------------------------------------------------------===//

#include "include/setjmp.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcSetjmpUnderscoreNamesTest, MacrosAreDefined) {
#ifndef _setjmp
  FAIL() << "_setjmp is not defined";
#endif
#ifndef _longjmp
  FAIL() << "_longjmp is not defined";
#endif
}
