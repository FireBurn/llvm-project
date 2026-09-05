//===-- Unittests for the stream lock -------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/stdio/fclose.h"
#include "src/stdio/fgetc.h"
#include "src/stdio/flockfile.h"
#include "src/stdio/fopen.h"
#include "src/stdio/fread.h"
#include "src/stdio/ftrylockfile.h"
#include "src/stdio/funlockfile.h"
#include "src/stdio/fwrite.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcFileLockTest, LockIsRecursive) {
  constexpr char FILENAME[] = "testdata/filelock_recursive.test";
  ::FILE *f = LIBC_NAMESPACE::fopen(FILENAME, "w");
  ASSERT_FALSE(f == nullptr);
  constexpr char CONTENT[] = "abcdef";
  ASSERT_EQ(sizeof(CONTENT) - 1,
            LIBC_NAMESPACE::fwrite(CONTENT, 1, sizeof(CONTENT) - 1, f));
  ASSERT_EQ(0, LIBC_NAMESPACE::fclose(f));

  f = LIBC_NAMESPACE::fopen(FILENAME, "r");
  ASSERT_FALSE(f == nullptr);

  // POSIX requires the lock to be recursive. Taking it more than once and
  // then making calls that take it themselves has to work, because that is
  // what a caller doing its own locking around ordinary stdio calls does.
  LIBC_NAMESPACE::flockfile(f);
  LIBC_NAMESPACE::flockfile(f);
  LIBC_NAMESPACE::flockfile(f);

  ASSERT_EQ(int('a'), LIBC_NAMESPACE::fgetc(f));
  char buffer[3];
  ASSERT_EQ(size_t(3), LIBC_NAMESPACE::fread(buffer, 1, 3, f));
  ASSERT_EQ(buffer[0], char('b'));
  ASSERT_EQ(buffer[2], char('d'));

  LIBC_NAMESPACE::funlockfile(f);
  LIBC_NAMESPACE::funlockfile(f);
  LIBC_NAMESPACE::funlockfile(f);

  ASSERT_EQ(0, LIBC_NAMESPACE::fclose(f));
}

TEST(LlvmLibcFileLockTest, TryLockNestsOnTheSameThread) {
  constexpr char FILENAME[] = "testdata/filelock_trylock.test";
  ::FILE *f = LIBC_NAMESPACE::fopen(FILENAME, "w");
  ASSERT_FALSE(f == nullptr);

  // The lock is already held by this thread, so asking again succeeds rather
  // than reporting it busy.
  ASSERT_EQ(0, LIBC_NAMESPACE::ftrylockfile(f));
  ASSERT_EQ(0, LIBC_NAMESPACE::ftrylockfile(f));
  LIBC_NAMESPACE::funlockfile(f);
  LIBC_NAMESPACE::funlockfile(f);

  ASSERT_EQ(0, LIBC_NAMESPACE::fclose(f));
}
