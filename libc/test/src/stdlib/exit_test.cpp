//===-- Unittests for exit -----------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/types/FILE.h"
#include "src/__support/CPP/string_view.h"
#include "src/stdio/fclose.h"
#include "src/stdio/fopen.h"
#include "src/stdio/fputs.h"
#include "src/stdio/fread.h"
#include "src/stdio/remove.h"
#include "src/stdlib/exit.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcStdlib, exit) {
  EXPECT_EXITS([] { LIBC_NAMESPACE::exit(1); }, 1);
  EXPECT_EXITS([] { LIBC_NAMESPACE::exit(65); }, 65);
}

TEST(LlvmLibcStdlib, ExitFlushesOpenStreams) {
  constexpr const char *FILENAME = "exit_flush.test";
  constexpr const char *CONTENTS = "written but never closed";
  LIBC_NAMESPACE::remove(FILENAME);

  // The child writes to a stream and exits without closing it. What it wrote
  // is short enough to still be sitting in the buffer.
  EXPECT_EXITS(
      [] {
        ::FILE *f = LIBC_NAMESPACE::fopen("exit_flush.test", "w");
        if (f == nullptr)
          LIBC_NAMESPACE::exit(1);
        LIBC_NAMESPACE::fputs("written but never closed", f);
        LIBC_NAMESPACE::exit(0);
      },
      0);

  ::FILE *f = LIBC_NAMESPACE::fopen(FILENAME, "r");
  ASSERT_TRUE(f != nullptr);
  char buf[64] = {};
  size_t read = LIBC_NAMESPACE::fread(buf, 1, sizeof(buf) - 1, f);
  ASSERT_EQ(LIBC_NAMESPACE::fclose(f), 0);
  EXPECT_TRUE(LIBC_NAMESPACE::cpp::string_view(buf, read) ==
              LIBC_NAMESPACE::cpp::string_view(CONTENTS));

  ASSERT_EQ(LIBC_NAMESPACE::remove(FILENAME), 0);
}
