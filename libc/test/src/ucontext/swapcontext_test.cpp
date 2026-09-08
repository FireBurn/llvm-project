//===-- Unittests for makecontext and swapcontext -------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/ucontext/getcontext.h"
#include "src/ucontext/makecontext.h"
#include "src/ucontext/swapcontext.h"
#include "test/UnitTest/Test.h"

#include <ucontext.h>

namespace {

// The stack the made context runs on. It belongs to that context alone for
// as long as it is running, so it is not shared between the tests.
alignas(16) char stack[65536];

ucontext_t made;
ucontext_t caller;

volatile int reached = 0;
volatile int first_argument = 0;
volatile int second_argument = 0;

void entry() { reached = 1; }

void entry_with_arguments(int first, int second) {
  first_argument = first;
  second_argument = second;
  reached = 2;
}

} // anonymous namespace

// swapcontext saves where it was called and goes to the other context, which
// comes back the same way when the function it was made with returns.
TEST(LlvmLibcSwapcontextTest, GoesToAMadeContextAndComesBack) {
  reached = 0;
  ASSERT_EQ(LIBC_NAMESPACE::getcontext(&made), 0);
  made.uc_stack.ss_sp = stack;
  made.uc_stack.ss_size = sizeof(stack);
  made.uc_link = &caller;
  LIBC_NAMESPACE::makecontext(&made, &entry, 0);

  ASSERT_EQ(LIBC_NAMESPACE::swapcontext(&caller, &made), 0);
  ASSERT_EQ(reached, 1);
}

// The arguments given to makecontext reach the function it was made with.
TEST(LlvmLibcSwapcontextTest, PassesTheArgumentsItWasGiven) {
  reached = 0;
  first_argument = 0;
  second_argument = 0;

  ASSERT_EQ(LIBC_NAMESPACE::getcontext(&made), 0);
  made.uc_stack.ss_sp = stack;
  made.uc_stack.ss_size = sizeof(stack);
  made.uc_link = &caller;
  LIBC_NAMESPACE::makecontext(
      &made, reinterpret_cast<void (*)()>(&entry_with_arguments), 2, 11, 22);

  ASSERT_EQ(LIBC_NAMESPACE::swapcontext(&caller, &made), 0);
  ASSERT_EQ(reached, 2);
  ASSERT_EQ(first_argument, 11);
  ASSERT_EQ(second_argument, 22);
}
