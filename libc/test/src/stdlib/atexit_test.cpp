//===-- Unittests for atexit ----------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/__support/CPP/array.h"
#include "src/__support/CPP/utility.h"
#include "src/stdlib/_Exit.h"
#include "src/stdlib/atexit.h"
#include "src/stdlib/exit.h"
#include "test/UnitTest/Test.h"

static int a;
TEST(LlvmLibcAtExit, Basic) {
  // In case tests ever run multiple times.
  a = 0;

  auto test = [] {
    int status = LIBC_NAMESPACE::atexit(+[] {
      if (a != 1)
        __builtin_trap();
    });
    status |= LIBC_NAMESPACE::atexit(+[] { a++; });
    if (status)
      __builtin_trap();

    LIBC_NAMESPACE::exit(0);
  };
  EXPECT_EXITS(test, 0);
}

TEST(LlvmLibcAtExit, AtExitCallsSysExit) {
  auto test = [] {
    LIBC_NAMESPACE::atexit(+[] { LIBC_NAMESPACE::_Exit(1); });
    LIBC_NAMESPACE::exit(0);
  };
  EXPECT_EXITS(test, 1);
}

static int size;
static LIBC_NAMESPACE::cpp::array<int, 256> arr;

template <int... Ts>
void register_atexit_handlers(
    LIBC_NAMESPACE::cpp::integer_sequence<int, Ts...>) {
  (LIBC_NAMESPACE::atexit(+[] { arr[size++] = Ts; }), ...);
}

template <int count> constexpr auto getTest() {
  return [] {
    LIBC_NAMESPACE::atexit(+[] {
      if (size != count)
        __builtin_trap();
      for (int i = 0; i < count; i++)
        if (arr[i] != count - 1 - i)
          __builtin_trap();
    });
    register_atexit_handlers(
        LIBC_NAMESPACE::cpp::make_integer_sequence<int, count>{});
    LIBC_NAMESPACE::exit(0);
  };
}

TEST(LlvmLibcAtExit, ReverseOrder) {
  // In case tests ever run multiple times.
  size = 0;

  auto test = getTest<32>();
  EXPECT_EXITS(test, 0);
}

TEST(LlvmLibcAtExit, Many) {
  // In case tests ever run multiple times.
  size = 0;

  auto test = getTest<256>();
  EXPECT_EXITS(test, 0);
}

TEST(LlvmLibcAtExit, HandlerCallsAtExit) {
  auto test = [] {
    LIBC_NAMESPACE::atexit(
        +[] { LIBC_NAMESPACE::atexit(+[] { LIBC_NAMESPACE::exit(1); }); });
    LIBC_NAMESPACE::exit(0);
  };
  EXPECT_EXITS(test, 1);
}

extern "C" int __cxa_atexit(void (*)(void *), void *, void *);
extern "C" void __cxa_finalize(void *);

static int unloaded_module, other_module;
static int order[4];
static int ran;

static void record(void *which) { order[ran++] = *static_cast<int *>(which); }

TEST(LlvmLibcAtExit, FinalizeRunsOnlyWhatTheObjectRegistered) {
  ran = 0;
  auto test = [] {
    static int first = 1, second = 2, other = 3;
    // At exit, what was left: only the other object's handler, and nothing
    // of the one already finalized ran twice.
    LIBC_NAMESPACE::atexit(+[] {
      if (ran != 3 || order[2] != 3)
        __builtin_trap();
    });
    __cxa_atexit(record, &other, &other_module);
    __cxa_atexit(record, &first, &unloaded_module);
    __cxa_atexit(record, &second, &unloaded_module);

    __cxa_finalize(&unloaded_module);
    if (ran != 2 || order[0] != 2 || order[1] != 1)
      __builtin_trap();
    __cxa_finalize(&unloaded_module);
    if (ran != 2)
      __builtin_trap();
    LIBC_NAMESPACE::exit(0);
  };
  EXPECT_EXITS(test, 0);
}
