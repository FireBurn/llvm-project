//===-- String type specifier converters for scanf --------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_STDIO_SCANF_CORE_STRING_CONVERTER_H
#define LLVM_LIBC_SRC_STDIO_SCANF_CORE_STRING_CONVERTER_H

#include "src/__support/CPP/limits.h"
#include "src/__support/ctype_utils.h"
#include "src/__support/macros/config.h"
#include "src/stdio/scanf_core/core_structs.h"
#include "src/stdio/scanf_core/reader.h"

#include "hdr/func/free.h"
#include "hdr/func/realloc.h"

#include <stddef.h>

namespace LIBC_NAMESPACE_DECL {
namespace scanf_core {

template <typename T>
int convert_string(Reader<T> *reader, const FormatSection &to_conv) {
  // %s "Matches a sequence of non-white-space characters"

  // %c "Matches a sequence of characters of exactly the number specified by the
  // field width (1 if no field width is present in the directive)"

  // %[ "Matches a nonempty sequence of characters from a set of expected
  // characters (the scanset)."
  size_t max_width = 0;
  if (to_conv.max_width > 0) {
    max_width = to_conv.max_width;
  } else {
    if (to_conv.conv_name == 'c') {
      max_width = 1;
    } else {
      max_width = cpp::numeric_limits<size_t>::max();
    }
  }

  // With the allocating conversion the caller gives somewhere to put a
  // pointer rather than somewhere to put the characters, and how much room
  // there is to be is this function's to decide.
  const bool allocating =
      (to_conv.flags & ALLOCATE) != 0 && (to_conv.flags & NO_WRITE) == 0;
  const bool writing = (to_conv.flags & NO_WRITE) == 0;
  // Everything but %c is terminated, so there has to be room for that too.
  const size_t terminator = to_conv.conv_name == 'c' ? 0 : 1;

  char *output =
      allocating ? nullptr : reinterpret_cast<char *>(to_conv.output_ptr);
  size_t capacity = 0;

  char cur_char = reader->getc();
  size_t i = 0;
  for (; i < max_width && cur_char != '\0'; ++i) {
    // If this is %s and we've hit a space, or if this is %[] and we've found
    // something not in the scanset.
    if ((to_conv.conv_name == 's' && internal::isspace(cur_char)) ||
        (to_conv.conv_name == '[' && !to_conv.scan_set.test(cur_char))) {
      break;
    }
    if (allocating && i + terminator + 1 > capacity) {
      const size_t wanted = capacity == 0 ? 32 : capacity * 2;
      char *grown = reinterpret_cast<char *>(::realloc(output, wanted));
      if (grown == nullptr) {
        ::free(output);
        reader->ungetc(cur_char);
        return ALLOCATION_FAILURE;
      }
      output = grown;
      capacity = wanted;
    }
    // if the NO_WRITE flag is not set, write to the output.
    if (writing)
      output[i] = cur_char;
    cur_char = reader->getc();
  }

  // We always read one more character than will be used, so we have to put the
  // last one back.
  reader->ungetc(cur_char);

  // %c takes exactly as many characters as the width says, so input that ran
  // out before then matched nothing at all.
  const bool short_read = to_conv.conv_name == 'c' && i < max_width;

  if (i == 0 || short_read) {
    if (allocating)
      ::free(output);
    return short_read ? INPUT_FAILURE : MATCHING_FAILURE;
  }

  // If this is %s or %[]
  if (terminator != 0 && writing) {
    // Always null terminate the string. This may cause a write to the
    // (max_width + 1) byte, which is correct. The max width describes the max
    // number of characters read from the input string, and doesn't necessarily
    // correspond to the output.
    output[i] = '\0';
  }

  if (allocating)
    *reinterpret_cast<char **>(to_conv.output_ptr) = output;

  return READ_OK;
}

} // namespace scanf_core
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_STDIO_SCANF_CORE_STRING_CONVERTER_H
