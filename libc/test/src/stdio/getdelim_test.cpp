//===-- Unittests for getdelim and getline --------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/func/free.h"
#include "src/__support/CPP/scope.h"
#include "src/__support/libc_errno.h"
#include "src/stdio/fclose.h"
#include "src/stdio/fopen.h"
#include "src/stdio/fwrite.h"
#include "src/stdio/getdelim.h"
#include "src/stdio/getline.h"
#include "src/string/strcmp.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

using LlvmLibcGetdelimTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;
using namespace LIBC_NAMESPACE::testing::ErrnoSetterMatcher;
using LIBC_NAMESPACE::cpp::scope_exit;

namespace {

constexpr char CONTENT[] = "line one\n"
                           "\n"
                           "a considerably longer third line to force a grow\n"
                           "no trailing newline";
constexpr size_t CONTENT_SIZE = sizeof(CONTENT) - 1;

// Returns the number of bytes written, so the caller can assert on it.
size_t write_test_file(const char *name) {
  ::FILE *file = LIBC_NAMESPACE::fopen(name, "w");
  if (file == nullptr)
    return 0;
  size_t written = LIBC_NAMESPACE::fwrite(CONTENT, 1, CONTENT_SIZE, file);
  if (LIBC_NAMESPACE::fclose(file) != 0)
    return 0;
  return written;
}

} // anonymous namespace

TEST_F(LlvmLibcGetdelimTest, ReadLines) {
  constexpr char FILENAME[] = "testdata/getdelim.test";
  ASSERT_EQ(write_test_file(FILENAME), CONTENT_SIZE);

  ::FILE *file = LIBC_NAMESPACE::fopen(FILENAME, "r");
  ASSERT_FALSE(file == nullptr);
  scope_exit close_file(
      [&] { ASSERT_THAT(LIBC_NAMESPACE::fclose(file), Succeeds()); });

  char *buf = nullptr;
  size_t n = 0;
  scope_exit free_buf([&] { ::free(buf); });

  // getline allocates on the first call and keeps the delimiter.
  ASSERT_EQ(LIBC_NAMESPACE::getline(&buf, &n, file), ssize_t(9));
  ASSERT_FALSE(buf == nullptr);
  ASSERT_EQ(LIBC_NAMESPACE::strcmp(buf, "line one\n"), 0);

  // An empty line is one character, not end of input.
  ASSERT_EQ(LIBC_NAMESPACE::getline(&buf, &n, file), ssize_t(1));
  ASSERT_EQ(LIBC_NAMESPACE::strcmp(buf, "\n"), 0);

  // A line longer than the current buffer forces a reallocation.
  ASSERT_EQ(LIBC_NAMESPACE::getline(&buf, &n, file), ssize_t(49));
  ASSERT_GE(n, size_t(50));

  // The final line has no delimiter but is still returned.
  ASSERT_EQ(LIBC_NAMESPACE::getline(&buf, &n, file), ssize_t(19));
  ASSERT_EQ(LIBC_NAMESPACE::strcmp(buf, "no trailing newline"), 0);

  // End of input.
  ASSERT_EQ(LIBC_NAMESPACE::getline(&buf, &n, file), ssize_t(-1));
}

TEST_F(LlvmLibcGetdelimTest, CustomDelimiter) {
  constexpr char FILENAME[] = "testdata/getdelim_delim.test";
  ASSERT_EQ(write_test_file(FILENAME), CONTENT_SIZE);

  ::FILE *file = LIBC_NAMESPACE::fopen(FILENAME, "r");
  ASSERT_FALSE(file == nullptr);
  scope_exit close_file(
      [&] { ASSERT_THAT(LIBC_NAMESPACE::fclose(file), Succeeds()); });

  char *buf = nullptr;
  size_t n = 0;
  scope_exit free_buf([&] { ::free(buf); });

  ASSERT_EQ(LIBC_NAMESPACE::getdelim(&buf, &n, 'o', file), ssize_t(6));
  ASSERT_EQ(LIBC_NAMESPACE::strcmp(buf, "line o"), 0);
}

TEST_F(LlvmLibcGetdelimTest, NullArguments) {
  constexpr char FILENAME[] = "testdata/getdelim_null.test";
  ASSERT_EQ(write_test_file(FILENAME), CONTENT_SIZE);

  ::FILE *file = LIBC_NAMESPACE::fopen(FILENAME, "r");
  ASSERT_FALSE(file == nullptr);
  scope_exit close_file(
      [&] { ASSERT_THAT(LIBC_NAMESPACE::fclose(file), Succeeds()); });

  char *buf = nullptr;
  size_t n = 0;

  ASSERT_EQ(LIBC_NAMESPACE::getdelim(nullptr, &n, '\n', file), ssize_t(-1));
  ASSERT_ERRNO_EQ(EINVAL);
  libc_errno = 0;

  ASSERT_EQ(LIBC_NAMESPACE::getdelim(&buf, nullptr, '\n', file), ssize_t(-1));
  ASSERT_ERRNO_EQ(EINVAL);
  libc_errno = 0;
}
