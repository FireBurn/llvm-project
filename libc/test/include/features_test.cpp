//===-- Unittests for the feature test macros -----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "include/__llvm-libc-common.h"
#include "test/UnitTest/Test.h"

// A C++ compiler targeting Linux defines _GNU_SOURCE of its own accord, so
// this file lands in the case where an application has asked for everything.
#ifndef _GNU_SOURCE
#error "this test expects to be compiled with _GNU_SOURCE"
#endif

#ifdef _DEFAULT_SOURCE
static constexpr bool default_source_set = true;
#else
static constexpr bool default_source_set = false;
#endif

#ifdef _POSIX_SOURCE
static constexpr bool posix_source_set = true;
#else
static constexpr bool posix_source_set = false;
#endif

#ifdef _POSIX_C_SOURCE
static constexpr long posix_c_source_value = _POSIX_C_SOURCE;
#else
static constexpr long posix_c_source_value = 0;
#endif

#ifdef _XOPEN_SOURCE
static constexpr long xopen_source_value = _XOPEN_SOURCE;
#else
static constexpr long xopen_source_value = 0;
#endif

// Asking for everything selects the whole set, not only the GNU part of it.
TEST(LlvmLibcFeaturesTest, GnuSourceSelectsEverything) {
  EXPECT_TRUE(default_source_set);
  EXPECT_TRUE(posix_source_set);
  EXPECT_GE(posix_c_source_value, 200809L);
  EXPECT_GE(xopen_source_value, 700L);
}
