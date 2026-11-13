//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Tests the declarations of the public android/fdsan.h header.
///
//===----------------------------------------------------------------------===//

#include "include/android/fdsan.h"
#include "test/UnitTest/Test.h"

// bionic declares these two weak, and Android code tests their address to see
// whether the C library it runs on has them. A strong declaration makes the
// compiler decide the answer at build time.
TEST(LlvmLibcAndroidFdsanTest, ExchangeAndCloseAreWeakDeclarations) {
  EXPECT_EQ(__builtin_constant_p(&android_fdsan_exchange_owner_tag != nullptr),
            0);
  EXPECT_EQ(__builtin_constant_p(&android_fdsan_close_with_tag != nullptr), 0);
}
