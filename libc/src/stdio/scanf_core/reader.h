//===-- Reader definition for scanf -----------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_STDIO_SCANF_CORE_READER_H
#define LLVM_LIBC_SRC_STDIO_SCANF_CORE_READER_H

#include "src/__support/ctype_utils.h"
#include "src/__support/macros/attributes.h" // For LIBC_INLINE
#include "src/__support/macros/config.h"

#include <stddef.h>

namespace LIBC_NAMESPACE_DECL {
namespace scanf_core {

template <typename Derived> class Reader {
  size_t cur_chars_read = 0;
  bool input_exhausted = false;
  bool read_non_space = false;

public:
  // This returns the next character from the input and advances it by one
  // character. When it hits the end of the string or file it returns '\0' to
  // signal to stop parsing.
  LIBC_INLINE char getc() {
    ++cur_chars_read;
    char c = static_cast<Derived *>(this)->getc();
    if (c == '\0')
      input_exhausted = true;
    else if (!internal::isspace(static_cast<unsigned char>(c)))
      read_non_space = true;
    return c;
  }

  // This moves the input back by one character, placing c into the buffer if
  // this is a file reader, else c is ignored.
  LIBC_INLINE void ungetc(int c) {
    --cur_chars_read;
    static_cast<Derived *>(this)->ungetc(c);
  }

  LIBC_INLINE size_t chars_read() { return cur_chars_read; }

  // Called before each directive, so the two below describe just the one
  // being worked on.
  LIBC_INLINE void begin_directive() {
    input_exhausted = false;
    read_non_space = false;
  }

  // Whether this directive ran out of input.
  LIBC_INLINE bool hit_end_of_input() const { return input_exhausted; }

  // Whether it got anything but white space before it did. A directive which
  // reached the end of the input without ever finding a character to work on
  // failed for want of input rather than because the input did not match.
  LIBC_INLINE bool took_non_space() const { return read_non_space; }
};

} // namespace scanf_core
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_STDIO_SCANF_CORE_READER_H
