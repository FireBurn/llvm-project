//===-- Unittests for strerror_l --------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for strerror_l.
///
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/locale_macros.h"
#include "src/string/strerror.h"
#include "src/string/strerror_l.h"
#include "src/string/strcmp.h"
#include "test/UnitTest/Test.h"

// The messages are the same in every locale here, so the call answers as
// strerror does.
TEST(LlvmLibcStrerrorLTest, MatchesStrerror) {
  const int errors[] = {0, EPERM, ENOENT, EDOM, ERANGE, 12345};
  for (int err : errors) {
    ASSERT_STREQ(LIBC_NAMESPACE::strerror_l(err, LC_GLOBAL_LOCALE),
                 LIBC_NAMESPACE::strerror(err));
  }
}

TEST(LlvmLibcStrerrorLTest, NamesTheError) {
  ASSERT_STREQ(LIBC_NAMESPACE::strerror_l(ENOENT, LC_GLOBAL_LOCALE),
               "No such file or directory");
}
