//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for __libc_current_sigrtmin, __libc_current_sigrtmax and
/// __ctype_get_mb_cur_max
///
//===----------------------------------------------------------------------===//

#include "hdr/signal_macros.h"
#include "src/compat/__ctype_get_mb_cur_max.h"
#include "src/compat/__libc_current_sigrtmax.h"
#include "src/compat/__libc_current_sigrtmin.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcCompatTest, TheRealTimeRangeIsTheOneInSignalH) {
  ASSERT_EQ(LIBC_NAMESPACE::__libc_current_sigrtmin(), SIGRTMIN);
  ASSERT_EQ(LIBC_NAMESPACE::__libc_current_sigrtmax(), SIGRTMAX);
  ASSERT_LT(LIBC_NAMESPACE::__libc_current_sigrtmin(),
            LIBC_NAMESPACE::__libc_current_sigrtmax());
}

// The C locale takes one byte for a character.
TEST(LlvmLibcCompatTest, TheCLocaleHasSingleByteCharacters) {
  ASSERT_EQ(LIBC_NAMESPACE::__ctype_get_mb_cur_max(), size_t(1));
}
