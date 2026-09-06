//===-- Test that the environment follows environ -------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/stdlib/getenv.h"
#include "src/stdlib/setenv.h"
#include "src/unistd/environ.h"

#include "test/IntegrationTest/test.h"

TEST_MAIN([[maybe_unused]] int argc, [[maybe_unused]] char **argv,
          [[maybe_unused]] char **envp) {
  char **saved = LIBC_NAMESPACE::environ;

  // POSIX makes environ the environment itself, so a program may point it at
  // an array of its own and everything has to read from there.
  char first[] = "LLVM_LIBC_ONE=one";
  char second[] = "LLVM_LIBC_TWO=two";
  char *replacement[] = {first, second, nullptr};
  LIBC_NAMESPACE::environ = replacement;

  ASSERT_STREQ(LIBC_NAMESPACE::getenv("LLVM_LIBC_ONE"), "one");
  ASSERT_STREQ(LIBC_NAMESPACE::getenv("LLVM_LIBC_TWO"), "two");
  // The value points into the caller's own string rather than a copy.
  ASSERT_TRUE(LIBC_NAMESPACE::getenv("LLVM_LIBC_ONE") == first + 14);
  // Nothing from the environment it replaced is left.
  ASSERT_TRUE(LIBC_NAMESPACE::getenv("PATH") == nullptr);

  // Adding to the adopted environment keeps what was already in it.
  ASSERT_EQ(LIBC_NAMESPACE::setenv("LLVM_LIBC_THREE", "three", 1), 0);
  ASSERT_STREQ(LIBC_NAMESPACE::getenv("LLVM_LIBC_ONE"), "one");
  ASSERT_STREQ(LIBC_NAMESPACE::getenv("LLVM_LIBC_THREE"), "three");

  // Putting the array back leaves only what it holds.
  LIBC_NAMESPACE::environ = replacement;
  ASSERT_TRUE(LIBC_NAMESPACE::getenv("LLVM_LIBC_THREE") == nullptr);

  char *empty[] = {nullptr};
  LIBC_NAMESPACE::environ = empty;
  ASSERT_TRUE(LIBC_NAMESPACE::getenv("LLVM_LIBC_ONE") == nullptr);
  ASSERT_EQ(LIBC_NAMESPACE::setenv("LLVM_LIBC_ONLY", "only", 1), 0);
  ASSERT_STREQ(LIBC_NAMESPACE::getenv("LLVM_LIBC_ONLY"), "only");

  // A program may also shorten the array in place, without moving it, which
  // is what one that drops the variables it will not pass on does. The array
  // ends where its null is, not where it was last time.
  char a[] = "LLVM_LIBC_A=a";
  char b[] = "LLVM_LIBC_B=b";
  char c[] = "LLVM_LIBC_C=c";
  char *editable[] = {a, b, c, nullptr};
  LIBC_NAMESPACE::environ = editable;
  ASSERT_STREQ(LIBC_NAMESPACE::getenv("LLVM_LIBC_C"), "c");

  // Take the middle one out, the way removing an entry does.
  editable[1] = editable[2];
  editable[2] = nullptr;
  ASSERT_STREQ(LIBC_NAMESPACE::getenv("LLVM_LIBC_A"), "a");
  ASSERT_STREQ(LIBC_NAMESPACE::getenv("LLVM_LIBC_C"), "c");
  ASSERT_TRUE(LIBC_NAMESPACE::getenv("LLVM_LIBC_B") == nullptr);

  // And emptied entirely.
  editable[0] = nullptr;
  ASSERT_TRUE(LIBC_NAMESPACE::getenv("LLVM_LIBC_A") == nullptr);

  LIBC_NAMESPACE::environ = saved;
  ASSERT_FALSE(LIBC_NAMESPACE::getenv("PATH") == nullptr);
  return 0;
}
