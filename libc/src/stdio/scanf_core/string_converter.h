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
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/stdio/scanf_core/core_structs.h"
#include "src/stdio/scanf_core/reader.h"

#include "hdr/errno_macros.h"
#include "hdr/func/free.h"
#include "hdr/func/realloc.h"
#include "hdr/types/wchar_t.h"
#include "src/__support/wchar/mbrtowc.h"
#include "src/__support/wchar/mbstate.h"

#include <stddef.h>

namespace LIBC_NAMESPACE_DECL {
namespace scanf_core {

// The l modifier on %s, %c and %[ stores wide characters, decoding them from
// the multibyte input. The field width counts characters, not bytes. A scanset
// names single bytes, so a character outside it is tested as byte 0xff: it is
// in an inverted set and not in one that was listed.
template <typename T>
int convert_wide_string(Reader<T> *reader, const FormatSection &to_conv) {
  size_t max_width = 0;
  if (to_conv.max_width > 0) {
    max_width = to_conv.max_width;
  } else {
    max_width =
        to_conv.conv_name == 'c' ? 1 : cpp::numeric_limits<size_t>::max();
  }

  const bool allocating =
      (to_conv.flags & ALLOCATE) != 0 && (to_conv.flags & NO_WRITE) == 0;
  const bool writing = (to_conv.flags & NO_WRITE) == 0;
  const size_t terminator = to_conv.conv_name == 'c' ? 0 : 1;

  wchar_t *output =
      allocating ? nullptr : reinterpret_cast<wchar_t *>(to_conv.output_ptr);
  size_t capacity = 0;
  size_t i = 0;
  bool encoding_error = false;

  for (; i < max_width; ++i) {
    char bytes[4];
    size_t count = 0;
    internal::mbstate state;
    wchar_t wc = 0;
    bool complete = false;
    bool ended = false;
    while (!complete && !encoding_error && !ended) {
      char c = reader->getc();
      if (reader->hit_end_of_input()) {
        reader->ungetc(c);
        ended = true;
        break;
      }
      bytes[count++] = c;
      auto used = internal::mbrtowc(&wc, &c, 1, &state);
      if (!used.has_value())
        encoding_error = true;
      else if (used.value() != static_cast<size_t>(-2))
        complete = true;
      else if (count == sizeof(bytes))
        encoding_error = true;
    }
    if (ended && count > 0)
      encoding_error = true;
    if (!complete)
      break;

    const bool is_space = wc < 0x80 && internal::isspace(static_cast<char>(wc));
    if ((to_conv.conv_name == 's' && is_space) ||
        (to_conv.conv_name == '[' &&
         !to_conv.scan_set.test(wc < 0x100 ? static_cast<unsigned char>(wc)
                                           : 0xff))) {
      while (count > 0)
        reader->ungetc(bytes[--count]);
      break;
    }
    if (allocating && i + terminator + 1 > capacity) {
      const size_t wanted = capacity == 0 ? 32 : capacity * 2;
      wchar_t *grown = reinterpret_cast<wchar_t *>(
          ::realloc(output, wanted * sizeof(wchar_t)));
      if (grown == nullptr) {
        ::free(output);
        return ALLOCATION_FAILURE;
      }
      output = grown;
      capacity = wanted;
    }
    if (writing)
      output[i] = wc;
  }

  if (encoding_error) {
    if (allocating)
      ::free(output);
    libc_errno = EILSEQ;
    return MATCHING_FAILURE;
  }

  const bool short_read = to_conv.conv_name == 'c' && i < max_width;
  if (i == 0 || short_read) {
    if (allocating)
      ::free(output);
    return short_read ? INPUT_FAILURE : MATCHING_FAILURE;
  }

  if (terminator != 0 && writing)
    output[i] = L'\0';

  if (allocating)
    *reinterpret_cast<wchar_t **>(to_conv.output_ptr) = output;

  return READ_OK;
}

template <typename T>
int convert_string(Reader<T> *reader, const FormatSection &to_conv) {
  if (to_conv.length_modifier == LengthModifier::l)
    return convert_wide_string(reader, to_conv);

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
  for (; i < max_width && !reader->hit_end_of_input(); ++i) {
    // If this is %s and we've hit a space, or if this is %[] and we've found
    // something not in the scanset.
    if ((to_conv.conv_name == 's' && internal::isspace(cur_char)) ||
        (to_conv.conv_name == '[' &&
         !to_conv.scan_set.test(static_cast<unsigned char>(cur_char)))) {
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
