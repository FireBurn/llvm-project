//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Tests that on Android the aarch64 signal frame records are visible through
/// mcontext_t's header, so a handler can find the FP/SIMD state.
///
//===----------------------------------------------------------------------===//

#ifndef __ANDROID__
#define __ANDROID__ 1
#endif

#include "include/llvm-libc-types/mcontext_t.h"
#include "test/UnitTest/Test.h"

#include <stddef.h>

static fpsimd_context *find_fpsimd(mcontext_t &context) {
  _aarch64_ctx *header = reinterpret_cast<_aarch64_ctx *>(&context.__reserved);
  while (header->magic != FPSIMD_MAGIC) {
    if (header->size == 0)
      return nullptr;
    header = reinterpret_cast<_aarch64_ctx *>(
        reinterpret_cast<char *>(header) + header->size);
  }
  return reinterpret_cast<fpsimd_context *>(header);
}

TEST(LlvmLibcAndroidAarch64SigcontextTest, FindsFpsimdRecord) {
  mcontext_t context = {};
  _aarch64_ctx *esr = reinterpret_cast<_aarch64_ctx *>(&context.__reserved);
  esr->magic = ESR_MAGIC;
  esr->size = sizeof(esr_context);
  fpsimd_context *fp = reinterpret_cast<fpsimd_context *>(
      reinterpret_cast<char *>(esr) + esr->size);
  fp->head.magic = FPSIMD_MAGIC;
  fp->head.size = sizeof(fpsimd_context);
  fp->vregs[31] = 42;

  fpsimd_context *found = find_fpsimd(context);
  ASSERT_TRUE(found == fp);
  EXPECT_EQ(static_cast<int>(found->vregs[31]), 42);
}
