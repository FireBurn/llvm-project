//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the public string.h header.
///
//===----------------------------------------------------------------------===//

#include "include/string.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcStringStrdupaTest, MacrosAreDefined) {
#ifndef strdupa
  FAIL() << "strdupa is not defined";
#endif
#ifndef strndupa
  FAIL() << "strndupa is not defined";
#endif
}
