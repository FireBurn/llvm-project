//===-- Unittests for the unlocked stream writers -------------------------===//
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
#include "src/stdio/fputc_unlocked.h"
#include "src/stdio/fputs_unlocked.h"
#include "src/stdio/fread.h"
#include "src/stdio/putc_unlocked.h"
#include "src/stdio/putchar_unlocked.h"
#include "src/stdio/remove.h"
#include "src/stdio/setbuffer.h"
#include "src/stdio/setlinebuf.h"
#include "src/stdio_ext/__flbf.h"
#include "src/string/memcmp.h"
#include "test/UnitTest/Test.h"

#include <stdio_ext.h>

// The unlocked writers do what the locked ones do, without taking the
// stream's lock. What is checked is that they write the same bytes.
TEST(LlvmLibcStdioUnlockedTest, WritesTheSameBytesAsTheLockedForms) {
  auto path = libc_make_test_file_path("stdio_unlocked.test");
  ::FILE *f = LIBC_NAMESPACE::fopen(path, "w");
  ASSERT_FALSE(f == nullptr);

  ASSERT_EQ(LIBC_NAMESPACE::fputc_unlocked('a', f), int('a'));
  ASSERT_EQ(LIBC_NAMESPACE::putc_unlocked('b', f), int('b'));
  ASSERT_GE(LIBC_NAMESPACE::fputs_unlocked("cde", f), 0);
  ASSERT_EQ(LIBC_NAMESPACE::fclose(f), 0);

  ::FILE *r = LIBC_NAMESPACE::fopen(path, "r");
  ASSERT_FALSE(r == nullptr);
  char buf[8] = {};
  ASSERT_EQ(LIBC_NAMESPACE::fread(buf, 1, 5, r), size_t(5));
  ASSERT_EQ(LIBC_NAMESPACE::memcmp(buf, "abcde", 5), 0);
  ASSERT_EQ(LIBC_NAMESPACE::fclose(r), 0);
  ASSERT_EQ(LIBC_NAMESPACE::remove(path), 0);
}

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

// setbuffer is setvbuf with the buffering decided by whether a buffer was
// given, and setlinebuf asks for line buffering and nothing else.
TEST(LlvmLibcStdioUnlockedTest, ChoosingHowTheStreamIsBuffered) {
  auto path = libc_make_test_file_path("stdio_buffer.test");
  ::FILE *f = LIBC_NAMESPACE::fopen(path, "w");
  ASSERT_FALSE(f == nullptr);

  ASSERT_EQ(LIBC_NAMESPACE::__flbf(f), 0);
  LIBC_NAMESPACE::setlinebuf(f);
  ASSERT_NE(LIBC_NAMESPACE::__flbf(f), 0);

  ASSERT_EQ(LIBC_NAMESPACE::fclose(f), 0);

  ::FILE *g = LIBC_NAMESPACE::fopen(path, "w");
  ASSERT_FALSE(g == nullptr);
  static char buffer[64];
  LIBC_NAMESPACE::setbuffer(g, buffer, sizeof(buffer));
  ASSERT_EQ(LIBC_NAMESPACE::__flbf(g), 0);
  ASSERT_GE(LIBC_NAMESPACE::fputs_unlocked("through the given buffer", g), 0);
  ASSERT_EQ(LIBC_NAMESPACE::fclose(g), 0);

  ASSERT_EQ(LIBC_NAMESPACE::remove(path), 0);
}

// putchar_unlocked writes to stdout, and reports the character it wrote.
TEST(LlvmLibcStdioUnlockedTest, WritesOneCharacterToStandardOutput) {
  ASSERT_EQ(LIBC_NAMESPACE::putchar_unlocked('\n'), int('\n'));
}
