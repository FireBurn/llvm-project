//===-- Unittests for the vector string loads on known arrays -------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

///
/// \file
/// The vector implementations of strlen and memchr read the whole aligned
/// vector around the string, outside the object on purpose. When a call is
/// inlined on an array the compiler can see, it treats those bytes as
/// undefined, so these call them directly on local arrays.
///
//===----------------------------------------------------------------------===//

#include "src/string/string_length.h"
#include "test/UnitTest/Test.h"

#if LIBC_HAS_VECTOR_TYPE

namespace {

using LIBC_NAMESPACE::clang_vector::find_first_character;
using LIBC_NAMESPACE::clang_vector::string_length;

// Kept in the caller so the compiler sees the whole object.
[[gnu::always_inline]] inline size_t length_of_local(unsigned shift) {
  char text[40];
  for (unsigned i = 0; i < sizeof(text); ++i)
    text[i] = static_cast<char>('a' + (i % 26));
  // Poison what follows the string, so that reading it shows.
  for (unsigned i = shift + 6; i < sizeof(text); ++i)
    text[i] = 'z';
  text[shift + 6] = '\0';
  return string_length(text + shift);
}

} // anonymous namespace

TEST(LlvmLibcInlineStrlenTest, StringLengthOnLocalArray) {
  for (unsigned shift = 0; shift < 16; ++shift)
    ASSERT_EQ(length_of_local(shift), size_t(6));
}

TEST(LlvmLibcInlineStrlenTest, FindFirstCharacterOnLocalArray) {
  unsigned char buffer[24];
  for (unsigned i = 0; i < sizeof(buffer); ++i)
    buffer[i] = static_cast<unsigned char>('a' + i);
  // Present within the range, and absent from it though it is further on.
  ASSERT_TRUE(find_first_character(buffer, 'd', sizeof(buffer)) ==
              buffer + 3);
  ASSERT_TRUE(find_first_character(buffer, 'x', 8) == nullptr);
  ASSERT_TRUE(find_first_character(buffer + 5, 'k', 4) == nullptr);
  ASSERT_TRUE(find_first_character(buffer + 5, 'k', 8) == buffer + 10);
}

#endif // LIBC_HAS_VECTOR_TYPE
