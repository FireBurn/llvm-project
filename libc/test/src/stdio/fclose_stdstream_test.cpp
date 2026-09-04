//===-- Unittest for closing a standard stream ----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/types/FILE.h"
#include "src/stdio/fclose.h"
#include "src/stdio/stdin.h"
#include "test/UnitTest/Test.h"

// The standard streams are statically allocated, so closing one must not hand
// it to the deallocator. fclose(stderr) is legal C, and doing this used to
// abort inside the allocator on a misaligned free.
//
// stdin is the one closed here so that the harness can still report the
// result on the streams it writes to.
TEST(LlvmLibcFcloseStdStreamTest, ClosingStdinDoesNotFreeIt) {
  ASSERT_EQ(
      LIBC_NAMESPACE::fclose(reinterpret_cast<::FILE *>(LIBC_NAMESPACE::stdin)),
      0);
}
