//===-- Unittests for LIBC_SHARED_INTERNAL --------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for LIBC_SHARED_INTERNAL.
///
//===----------------------------------------------------------------------===//

#include "src/__support/macros/config.h"
#include "test/UnitTest/Test.h"

#define LIBC_TEST_STRINGIFY_IMPL(X) #X
#define LIBC_TEST_STRINGIFY(X) LIBC_TEST_STRINGIFY_IMPL(X)

// Visible across a shared object edge in a shared library build, and nothing
// at all in any other.
TEST(LlvmLibcSharedInternalTest, ExpansionFollowsTheSharedLibraryOption) {
#ifdef LIBC_COPT_SHARED_LIBRARY
  EXPECT_STREQ(LIBC_TEST_STRINGIFY(LIBC_SHARED_INTERNAL),
               "__attribute__((visibility(\"default\")))");
#else
  EXPECT_STREQ(LIBC_TEST_STRINGIFY(LIBC_SHARED_INTERNAL), "");
#endif
}

namespace LIBC_NAMESPACE_DECL {
// A declaration carrying the macro is accepted as it stands.
LIBC_SHARED_INTERNAL int shared_internal_probe = 7;
} // namespace LIBC_NAMESPACE_DECL

TEST(LlvmLibcSharedInternalTest, ADeclarationCarryingItIsUsable) {
  EXPECT_EQ(LIBC_NAMESPACE::shared_internal_probe, 7);
}
