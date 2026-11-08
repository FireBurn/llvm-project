//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Tests the declaration macros sys/cdefs.h gives Android programs.
///
//===----------------------------------------------------------------------===//

#ifndef __ANDROID__
#define __ANDROID__ 1
#endif

#include "include/sys/cdefs.h"
#include "test/UnitTest/Test.h"

// The NDK's asset_manager.h declares AAsset_seek this way.
extern "C" long seek_like(int, long, int)
    __RENAME_IF_FILE_OFFSET64(seek_like64);
extern "C" long seek_like(int, long, int) { return 7; }

TEST(LlvmLibcAndroidSysCdefsTest, RenameIfFileOffset64LeavesTheNameAlone) {
  EXPECT_EQ(seek_like(0, 0, 0), 7L);
}
