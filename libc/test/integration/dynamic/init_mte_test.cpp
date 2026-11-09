//===-- Test of the tag mask before initialisers --------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Tests that the loader allows memory tags before it runs the initialisers
/// of the objects it loads. Those reach the allocator, and with the kernel's
/// default mask the allocator's tags and its pointers disagree on a CPU that
/// has MTE. Only the CPUs that have it are checked.
/// The program returns nonzero if any check fails.
///
//===----------------------------------------------------------------------===//

#include "check.h"

#include <sys/auxv.h>

#ifndef HWCAP2_MTE
#define HWCAP2_MTE (1 << 18)
#endif

// What the object's initialiser read. The program exports it, so that the
// object can reach it.
extern "C" long llvm_libc_mte_ctrl_in_init = -1;

extern "C" int main() {
  if (!(getauxval(AT_HWCAP2) & HWCAP2_MTE))
    return 0;

  constexpr long TAGGED_ADDR_ENABLE = 1;
  constexpr int TAG_SHIFT = 3;
  constexpr long TAG_MASK = 0xffffL << TAG_SHIFT;
  CHECK(llvm_libc_mte_ctrl_in_init >= 0);
  CHECK(llvm_libc_mte_ctrl_in_init & TAGGED_ADDR_ENABLE);
  CHECK(((llvm_libc_mte_ctrl_in_init & TAG_MASK) >> TAG_SHIFT) == 0xfffe);
  return failures == 0 ? 0 : 1;
}
