//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unit tests for error.
///
//===----------------------------------------------------------------------===//

#include "src/error/error.h"
#include "src/error/error_message_count.h"
#include "src/error/error_print_progname.h"
#include "test/UnitTest/Test.h"

// A status of zero is the caller saying it wants to carry on.
TEST(LlvmLibcErrorTest, ZeroStatusReturns) {
  unsigned int before = LIBC_NAMESPACE::error_message_count;
  LIBC_NAMESPACE::error(0, 0, "message %d", 1);
  EXPECT_EQ(LIBC_NAMESPACE::error_message_count, before + 1);
}

// An error number is turned into the text for it and added to the message.
TEST(LlvmLibcErrorTest, ErrorNumberIsReported) {
  unsigned int before = LIBC_NAMESPACE::error_message_count;
  LIBC_NAMESPACE::error(0, 2, "opening %s", "a file");
  EXPECT_EQ(LIBC_NAMESPACE::error_message_count, before + 1);
}

// There need not be a message at all.
TEST(LlvmLibcErrorTest, NullFormat) {
  unsigned int before = LIBC_NAMESPACE::error_message_count;
  LIBC_NAMESPACE::error(0, 0, nullptr);
  EXPECT_EQ(LIBC_NAMESPACE::error_message_count, before + 1);
}

static bool progname_hook_ran = false;
static void progname_hook(void) { progname_hook_ran = true; }

// Where the caller has supplied one, it writes the start of the message.
TEST(LlvmLibcErrorTest, PrintPrognameHook) {
  progname_hook_ran = false;
  LIBC_NAMESPACE::error_print_progname = &progname_hook;
  LIBC_NAMESPACE::error(0, 0, "hooked");
  LIBC_NAMESPACE::error_print_progname = nullptr;
  EXPECT_TRUE(progname_hook_ran);
}
