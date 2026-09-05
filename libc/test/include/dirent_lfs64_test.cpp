//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the public dirent.h header.
///
//===----------------------------------------------------------------------===//

#include "include/dirent.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcDirentLfs64Test, MacrosAreDefined) {
#ifndef dirent64
  FAIL() << "dirent64 is not defined";
#endif
#ifndef readdir64_r
  FAIL() << "readdir64_r is not defined";
#endif
}
