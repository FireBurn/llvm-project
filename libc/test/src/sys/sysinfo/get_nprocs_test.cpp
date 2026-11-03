//===-- Unittests for get_nprocs and get_nprocs_conf ------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for get_nprocs and get_nprocs_conf.
///
//===----------------------------------------------------------------------===//

#include "src/sched/sched_getaffinity.h"
#include "src/sys/sysinfo/get_nprocs.h"
#include "src/sys/sysinfo/get_nprocs_conf.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcGetNprocsTest, ThereIsAtLeastOneProcessor) {
  int online = LIBC_NAMESPACE::get_nprocs();
  int configured = LIBC_NAMESPACE::get_nprocs_conf();
  ASSERT_GE(online, 1);
  // Processors that are offline are configured but not online.
  ASSERT_GE(configured, online);
}
