//===-- Unittests for __fsetlocking ---------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/stdio_macros.h"
#include "hdr/types/FILE.h"
#include "src/stdio/__fsetlocking.h"
#include "src/stdio/fclose.h"
#include "src/stdio/fopen.h"
#include "src/stdio/remove.h"
#include "test/UnitTest/Test.h"

#include <stdio_ext.h>

// __fsetlocking reports the way the stream was locking before, whichever
// way it is asked to lock from now on.
TEST(LlvmLibcStdioUnlockedTest, SetsAndReportsTheLockingInForce) {
  auto path = libc_make_test_file_path("stdio_setlocking.test");
  ::FILE *f = LIBC_NAMESPACE::fopen(path, "w");
  ASSERT_FALSE(f == nullptr);

  ASSERT_EQ(LIBC_NAMESPACE::__fsetlocking(f, FSETLOCKING_QUERY),
            int(FSETLOCKING_INTERNAL));
  ASSERT_EQ(LIBC_NAMESPACE::__fsetlocking(f, FSETLOCKING_BYCALLER),
            int(FSETLOCKING_INTERNAL));
  ASSERT_EQ(LIBC_NAMESPACE::__fsetlocking(f, FSETLOCKING_QUERY),
            int(FSETLOCKING_BYCALLER));
  ASSERT_EQ(LIBC_NAMESPACE::__fsetlocking(f, FSETLOCKING_INTERNAL),
            int(FSETLOCKING_BYCALLER));

  ASSERT_EQ(LIBC_NAMESPACE::fclose(f), 0);
  ASSERT_EQ(LIBC_NAMESPACE::remove(path), 0);
}
