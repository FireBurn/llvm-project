//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Integration test for __progname and __progname_full.
///
//===----------------------------------------------------------------------===//

#include "src/compat/progname.h"
#include "src/errno/program_invocation_name.h"
#include "src/errno/program_invocation_short_name.h"
#include "test/IntegrationTest/test.h"

// The startup code sets the BSD names together with the glibc ones.
TEST_MAIN(int argc, char **argv, char **) {
  ASSERT_TRUE(argc >= 1);
  ASSERT_TRUE(LIBC_NAMESPACE::__progname_full == argv[0]);
  ASSERT_TRUE(LIBC_NAMESPACE::__progname_full ==
              LIBC_NAMESPACE::program_invocation_name);
  ASSERT_TRUE(LIBC_NAMESPACE::__progname ==
              LIBC_NAMESPACE::program_invocation_short_name);
  return 0;
}
