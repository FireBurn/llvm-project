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

extern "C" int aliased_target() { return 11; }
extern "C" int aliased_name();
__strong_alias(aliased_name, aliased_target)

TEST(LlvmLibcAndroidSysCdefsTest, StrongAliasNamesTheSameFunction) {
  EXPECT_EQ(aliased_name(), 11);
}

TEST(LlvmLibcAndroidSysCdefsTest, CastKeepsTheKindOfConversion) {
  const int value = 65;
  EXPECT_EQ(__BIONIC_CAST(static_cast, char, value), 'A');
  const void *pointer = &value;
  EXPECT_EQ(__BIONIC_CAST(reinterpret_cast, const int *, pointer), &value);
}

TEST(LlvmLibcAndroidSysCdefsTest, PredictionMacrosKeepTheValue) {
  int zero = 0;
  int five = 5;
  EXPECT_EQ(__predict_true(five), 1L);
  EXPECT_EQ(__predict_true(zero), 0L);
  EXPECT_EQ(__predict_false(five), 1L);
  EXPECT_EQ(__predict_false(zero), 0L);
}

// Android's own headers mark library-internal declarations with this.
extern "C" __LIBC_HIDDEN__ int hidden_like(int);
extern "C" int hidden_like(int v) { return v; }

TEST(LlvmLibcAndroidSysCdefsTest, LibcHiddenDeclaresAFunction) {
  EXPECT_EQ(hidden_like(3), 3);
}

// Android sources select their bionic code paths with this.
#ifndef __BIONIC__
#error "__BIONIC__ is not defined"
#endif

TEST(LlvmLibcAndroidSysCdefsTest, BionicIsDefined) {
  EXPECT_EQ(__BIONIC__, 1);
}
