//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
///
/// \file
/// Unittests for android_reset_stack_guards.
///
//===----------------------------------------------------------------------===//

#include "hdr/stdint_proxy.h"
#include "src/__support/macros/properties/architectures.h"
#include "src/android/android_reset_stack_guards.h"
#include "src/stdlib/exit.h"
#include "test/UnitTest/Test.h"

#ifdef LIBC_TARGET_ARCH_IS_X86_64
#include "src/__support/threads/tcb.h"
#endif

extern "C" uintptr_t __stack_chk_guard;

namespace {

// A frame that was entered before the canary changed fails its check when it
// returns, so these leave through exit, and carry no check of their own.
[[gnu::no_stack_protector]] void changes_the_global_canary() {
  const uintptr_t before = __stack_chk_guard;
  LIBC_NAMESPACE::android_reset_stack_guards();
  LIBC_NAMESPACE::exit(__stack_chk_guard != before ? 0 : 1);
}

#ifdef LIBC_TARGET_ARCH_IS_X86_64
[[gnu::no_stack_protector]] void keeps_the_thread_canary_in_step() {
  const uintptr_t before = LIBC_NAMESPACE::get_tcb()->stack_guard;
  LIBC_NAMESPACE::android_reset_stack_guards();
  const uintptr_t after = LIBC_NAMESPACE::get_tcb()->stack_guard;
  LIBC_NAMESPACE::exit(after != before && after == __stack_chk_guard ? 0 : 1);
}
#endif

} // anonymous namespace

TEST(LlvmLibcAndroidResetStackGuardsTest, TheGlobalCanaryIsReplaced) {
  EXPECT_EXITS(changes_the_global_canary, 0);
}

#ifdef LIBC_TARGET_ARCH_IS_X86_64
TEST(LlvmLibcAndroidResetStackGuardsTest, TheThreadCanaryFollowsTheGlobal) {
  EXPECT_EXITS(keeps_the_thread_canary_in_step, 0);
}
#endif
