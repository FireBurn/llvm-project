//===-- Utilities for suspending threads ----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC___SUPPORT_THREADS_SLEEP_H
#define LLVM_LIBC_SRC___SUPPORT_THREADS_SLEEP_H

#include "src/__support/macros/attributes.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

/// Suspend the thread briefly to assist the thread scheduler during busy loops.
LIBC_INLINE void sleep_briefly() {
#if defined(LIBC_TARGET_ARCH_IS_NVPTX)
  if (__nvvm_reflect("__CUDA_ARCH") >= 700)
    LIBC_INLINE_ASM("nanosleep.u32 64;" ::: "memory");
#elif defined(LIBC_TARGET_ARCH_IS_AMDGPU)
  __builtin_amdgcn_s_sleep(2);
#elif defined(LIBC_TARGET_ARCH_IS_X86)
  __builtin_ia32_pause();
#elif defined(LIBC_TARGET_ARCH_IS_AARCH64) && __has_builtin(__builtin_arm_isb)
  __builtin_arm_isb(0xf);
#elif defined(LIBC_TARGET_ARCH_IS_AARCH64)
  asm volatile("isb\n" ::: "memory");
#elif defined(LIBC_TARGET_ARCH_IS_ANY_RISCV)
  // The pause hint (Zihintpause), a nop where it is not implemented. The
  // volatile asm also stops the compiler from dropping a loop that only
  // calls this function.
  asm volatile(".4byte 0x0100000f" ::: "memory");
#else
  // No pause instruction here. Keep a barrier so that a spin loop around
  // this function still counts as having a side effect.
  asm volatile("" ::: "memory");
#endif
}

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC___SUPPORT_THREADS_SLEEP_H
