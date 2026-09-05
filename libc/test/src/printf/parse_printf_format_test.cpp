//===-- Unittests for parse_printf_format ---------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/printf_macros.h"
#include "src/printf/parse_printf_format.h"
#include "test/UnitTest/Test.h"

// The call says what each conversion in a format string takes, which is
// what a program that checks its own format strings asks for.
TEST(LlvmLibcParsePrintfFormatTest, ReportsTheTypeOfEachConversion) {
  int types[8] = {};
  size_t count = LIBC_NAMESPACE::parse_printf_format("%d %s %c %p", 8, types);
  ASSERT_EQ(count, size_t(4));
  ASSERT_EQ(types[0], PA_INT);
  ASSERT_EQ(types[1], PA_STRING);
  ASSERT_EQ(types[2], PA_CHAR);
  ASSERT_EQ(types[3], PA_POINTER);
}

// The length modifiers are reported as flags on the type rather than as
// types of their own.
TEST(LlvmLibcParsePrintfFormatTest, ReportsTheLengthAsAFlag) {
  int types[8] = {};
  size_t count =
      LIBC_NAMESPACE::parse_printf_format("%ld %lld %hd %f %Lf", 8, types);
  ASSERT_EQ(count, size_t(5));
  ASSERT_EQ(types[0], PA_INT | PA_FLAG_LONG);
  ASSERT_EQ(types[1], PA_INT | PA_FLAG_LONG_LONG);
  ASSERT_EQ(types[2], PA_INT | PA_FLAG_SHORT);
  ASSERT_EQ(types[3], PA_DOUBLE);
  ASSERT_EQ(types[4], PA_DOUBLE | PA_FLAG_LONG_DOUBLE);
}

// A width or precision given as a star is an argument of its own, and comes
// before the one it applies to.
TEST(LlvmLibcParsePrintfFormatTest, CountsTheStarsAsArguments) {
  int types[8] = {};
  size_t count = LIBC_NAMESPACE::parse_printf_format("%*.*d", 8, types);
  ASSERT_EQ(count, size_t(3));
  ASSERT_EQ(types[0], PA_INT);
  ASSERT_EQ(types[1], PA_INT);
  ASSERT_EQ(types[2], PA_INT);
}

// A doubled percent is a literal and takes nothing.
TEST(LlvmLibcParsePrintfFormatTest, ALiteralPercentTakesNothing) {
  int types[8] = {};
  ASSERT_EQ(LIBC_NAMESPACE::parse_printf_format("%% and %%", 8, types),
            size_t(0));
  ASSERT_EQ(LIBC_NAMESPACE::parse_printf_format("nothing at all", 8, types),
            size_t(0));
}

// The count of conversions is reported whether or not they fitted, so a
// caller can size an array and ask again.
TEST(LlvmLibcParsePrintfFormatTest, CountsMoreThanItWasGivenRoomFor) {
  int types[2] = {};
  size_t count = LIBC_NAMESPACE::parse_printf_format("%d %d %d %d", 2, types);
  ASSERT_EQ(count, size_t(4));
  ASSERT_EQ(types[0], PA_INT);
  ASSERT_EQ(types[1], PA_INT);

  ASSERT_EQ(LIBC_NAMESPACE::parse_printf_format("%d %d", 0, nullptr),
            size_t(2));
}
