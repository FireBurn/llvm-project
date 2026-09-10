//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for sys/dir.h bringing in dirent.h
///
//===----------------------------------------------------------------------===//

#include "include/sys/dir.h"
#include "test/UnitTest/Test.h"

#ifndef _LLVM_LIBC_DIRENT_H
#error "<sys/dir.h> does not bring in <dirent.h>"
#endif

TEST(LlvmLibcSysDirDirentTest, CarriesTheHeaders) {
  struct dirent entry;
  EXPECT_GT(sizeof(entry), size_t(0));
}
