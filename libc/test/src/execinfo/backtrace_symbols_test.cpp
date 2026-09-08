//===-- Unittests for backtrace_symbols -----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/execinfo/backtrace.h"
#include "src/execinfo/backtrace_symbols.h"
#include "src/stdlib/free.h"
#include "src/string/strlen.h"
#include "test/UnitTest/Test.h"

namespace {

// Three frames deep, so the trace has something in it to name. They are
// kept from being folded into one another.
[[gnu::noinline]] int innermost(void **buffer, int size) {
  int count = LIBC_NAMESPACE::backtrace(buffer, size);
  asm volatile("");
  return count;
}

[[gnu::noinline]] int middle(void **buffer, int size) {
  int count = innermost(buffer, size);
  asm volatile("");
  return count;
}

} // anonymous namespace

// Each address is named, in one block of memory the caller frees.
TEST(LlvmLibcBacktraceSymbolsTest, NamesEachAddress) {
  void *buffer[32] = {};
  int count = middle(buffer, 32);
  ASSERT_GE(count, 3);

  char **names = LIBC_NAMESPACE::backtrace_symbols(buffer, count);
  ASSERT_FALSE(names == nullptr);
  for (int i = 0; i < count; ++i) {
    ASSERT_FALSE(names[i] == nullptr);
    ASSERT_GT(LIBC_NAMESPACE::strlen(names[i]), size_t(0));
  }
  LIBC_NAMESPACE::free(names);
}

TEST(LlvmLibcBacktraceSymbolsTest, NothingToName) {
  void *buffer[1] = {};
  ASSERT_TRUE(LIBC_NAMESPACE::backtrace_symbols(buffer, 0) == nullptr);
}
