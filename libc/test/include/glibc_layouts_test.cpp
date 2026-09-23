//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the layout of lconv, regex, spawn, statvfs, fenv_t and nfds_t
///
//===----------------------------------------------------------------------===//

#include "include/locale.h"
#include "include/regex.h"
#include "include/spawn.h"
#include "include/sys/statvfs.h"
#include "include/fenv.h"
#include "include/poll.h"
#include "test/UnitTest/Test.h"

// The layouts are the ones glibc has, which programs built against it assume.
TEST(LlvmLibcGlibcLayoutsTest, MatchesGlibcOnX86_64) {
#if defined(__x86_64__) && defined(__linux__)
  EXPECT_EQ(sizeof(struct lconv), size_t(96));
  EXPECT_EQ(__builtin_offsetof(struct lconv, decimal_point), size_t(0));
  EXPECT_EQ(__builtin_offsetof(struct lconv, thousands_sep), size_t(8));
  EXPECT_EQ(__builtin_offsetof(struct lconv, grouping), size_t(16));
  EXPECT_EQ(__builtin_offsetof(struct lconv, int_curr_symbol), size_t(24));
  EXPECT_EQ(__builtin_offsetof(struct lconv, currency_symbol), size_t(32));
  EXPECT_EQ(__builtin_offsetof(struct lconv, mon_decimal_point), size_t(40));
  EXPECT_EQ(__builtin_offsetof(struct lconv, mon_thousands_sep), size_t(48));
  EXPECT_EQ(__builtin_offsetof(struct lconv, mon_grouping), size_t(56));
  EXPECT_EQ(__builtin_offsetof(struct lconv, positive_sign), size_t(64));
  EXPECT_EQ(__builtin_offsetof(struct lconv, negative_sign), size_t(72));
  EXPECT_EQ(__builtin_offsetof(struct lconv, int_frac_digits), size_t(80));
  EXPECT_EQ(__builtin_offsetof(struct lconv, frac_digits), size_t(81));
  EXPECT_EQ(__builtin_offsetof(struct lconv, p_cs_precedes), size_t(82));
  EXPECT_EQ(__builtin_offsetof(struct lconv, p_sep_by_space), size_t(83));
  EXPECT_EQ(__builtin_offsetof(struct lconv, n_cs_precedes), size_t(84));
  EXPECT_EQ(__builtin_offsetof(struct lconv, n_sep_by_space), size_t(85));
  EXPECT_EQ(__builtin_offsetof(struct lconv, p_sign_posn), size_t(86));
  EXPECT_EQ(__builtin_offsetof(struct lconv, n_sign_posn), size_t(87));
  EXPECT_EQ(__builtin_offsetof(struct lconv, int_p_cs_precedes), size_t(88));
  EXPECT_EQ(__builtin_offsetof(struct lconv, int_p_sep_by_space), size_t(89));
  EXPECT_EQ(__builtin_offsetof(struct lconv, int_n_cs_precedes), size_t(90));
  EXPECT_EQ(__builtin_offsetof(struct lconv, int_n_sep_by_space), size_t(91));
  EXPECT_EQ(__builtin_offsetof(struct lconv, int_p_sign_posn), size_t(92));
  EXPECT_EQ(__builtin_offsetof(struct lconv, int_n_sign_posn), size_t(93));
  EXPECT_EQ(sizeof(regex_t), size_t(64));
  EXPECT_EQ(__builtin_offsetof(regex_t, re_nsub), size_t(48));
  EXPECT_EQ(sizeof(regmatch_t), size_t(8));
  EXPECT_EQ(__builtin_offsetof(regmatch_t, rm_so), size_t(0));
  EXPECT_EQ(__builtin_offsetof(regmatch_t, rm_eo), size_t(4));
  EXPECT_EQ(sizeof(regoff_t), size_t(4));
  EXPECT_EQ(sizeof(posix_spawnattr_t), size_t(336));
  EXPECT_EQ(sizeof(posix_spawn_file_actions_t), size_t(80));
  EXPECT_EQ(sizeof(struct statvfs), size_t(112));
  EXPECT_EQ(__builtin_offsetof(struct statvfs, f_bsize), size_t(0));
  EXPECT_EQ(__builtin_offsetof(struct statvfs, f_frsize), size_t(8));
  EXPECT_EQ(__builtin_offsetof(struct statvfs, f_blocks), size_t(16));
  EXPECT_EQ(__builtin_offsetof(struct statvfs, f_bfree), size_t(24));
  EXPECT_EQ(__builtin_offsetof(struct statvfs, f_bavail), size_t(32));
  EXPECT_EQ(__builtin_offsetof(struct statvfs, f_files), size_t(40));
  EXPECT_EQ(__builtin_offsetof(struct statvfs, f_ffree), size_t(48));
  EXPECT_EQ(__builtin_offsetof(struct statvfs, f_favail), size_t(56));
  EXPECT_EQ(__builtin_offsetof(struct statvfs, f_fsid), size_t(64));
  EXPECT_EQ(__builtin_offsetof(struct statvfs, f_flag), size_t(72));
  EXPECT_EQ(__builtin_offsetof(struct statvfs, f_namemax), size_t(80));
  EXPECT_EQ(sizeof(fenv_t), size_t(32));
  EXPECT_EQ(sizeof(nfds_t), size_t(8));
#endif
}
