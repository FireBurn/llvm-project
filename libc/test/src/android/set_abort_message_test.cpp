//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for android_set_abort_message.
///
//===----------------------------------------------------------------------===//

#include "src/__support/abort_message.h"
#include "src/android/android_set_abort_message.h"
#include "src/string/strcmp.h"
#include "src/string/strlen.h"
#include "test/UnitTest/Test.h"

namespace abort_message = LIBC_NAMESPACE::abort_message;

TEST(LlvmLibcAndroidSetAbortMessageTest, MessageIsCopied) {
  char text[] = "first";
  LIBC_NAMESPACE::android_set_abort_message(text);
  text[0] = 'X';
  ASSERT_TRUE(abort_message::get() != nullptr);
  EXPECT_EQ(LIBC_NAMESPACE::strcmp(abort_message::get(), "first"), 0);
}

TEST(LlvmLibcAndroidSetAbortMessageTest, LaterMessageReplacesEarlier) {
  LIBC_NAMESPACE::android_set_abort_message("a long first message");
  LIBC_NAMESPACE::android_set_abort_message("second");
  EXPECT_EQ(LIBC_NAMESPACE::strcmp(abort_message::get(), "second"), 0);
}

TEST(LlvmLibcAndroidSetAbortMessageTest, NullKeepsTheMessage) {
  LIBC_NAMESPACE::android_set_abort_message("kept");
  LIBC_NAMESPACE::android_set_abort_message(nullptr);
  EXPECT_EQ(LIBC_NAMESPACE::strcmp(abort_message::get(), "kept"), 0);
}

TEST(LlvmLibcAndroidSetAbortMessageTest, LongMessageIsTruncated) {
  static char big[abort_message::CAPACITY * 2];
  for (size_t i = 0; i + 1 < sizeof(big); ++i)
    big[i] = 'a';
  big[sizeof(big) - 1] = '\0';
  LIBC_NAMESPACE::android_set_abort_message(big);
  EXPECT_EQ(LIBC_NAMESPACE::strlen(abort_message::get()),
            abort_message::CAPACITY - 1);
}

TEST(LlvmLibcAndroidSetAbortMessageTest, BlockSizeCountsItself) {
  LIBC_NAMESPACE::android_set_abort_message("four");
  const abort_message::Block &block = abort_message::block();
  EXPECT_EQ(block.size, sizeof(size_t) + 5);
  EXPECT_EQ(LIBC_NAMESPACE::strcmp(block.text, "four"), 0);
}
