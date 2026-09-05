//===-- Starting point for scanf --------------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_STDIO_SCANF_CORE_SCANF_MAIN_H
#define LLVM_LIBC_SRC_STDIO_SCANF_CORE_SCANF_MAIN_H

#include "hdr/errno_macros.h"
#include "hdr/stdio_macros.h"
#include "src/__support/arg_list.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/stdio/scanf_core/converter.h"
#include "src/stdio/scanf_core/core_structs.h"
#include "src/stdio/scanf_core/parser.h"
#include "src/stdio/scanf_core/reader.h"

#include <stddef.h>

namespace LIBC_NAMESPACE_DECL {
namespace scanf_core {

template <typename T>
int scanf_main(Reader<T> *reader, const char *__restrict str,
               internal::ArgList &args) {
  Parser<internal::ArgList> parser(str, args);
  int ret_val = READ_OK;
  int conversions = 0;
  for (FormatSection cur_section = parser.get_next_section();
       !cur_section.raw_string.empty() && ret_val == READ_OK;
       cur_section = parser.get_next_section()) {
    reader->begin_directive();
    if (cur_section.has_conv) {
      ret_val = convert(reader, cur_section);
      // What is counted is what was assigned. The %n (current position)
      // conversion assigns nothing of the input, and a suppressed one assigns
      // nothing at all, so neither counts.
      if (cur_section.conv_name != 'n' &&
          (cur_section.flags & FormatFlags::NO_WRITE) == 0)
        conversions += ret_val == READ_OK ? 1 : 0;
    } else {
      ret_val = raw_match(reader, cur_section.raw_string);
    }
  }

  // Somewhere to put an allocating conversion's result could not be had. The
  // count of what was assigned before that is of no use to the caller, since
  // it cannot tell it from an ordinary short read.
  if (ret_val == ALLOCATION_FAILURE) {
    libc_errno = ENOMEM;
    return EOF;
  }

  // A directive which failed for want of input, with nothing assigned yet, is
  // an input failure before any conversion, which C says is reported as EOF
  // rather than as a count of zero.
  if (conversions == 0 && ret_val != READ_OK &&
      (ret_val == INPUT_FAILURE ||
       (reader->hit_end_of_input() && !reader->took_non_space())))
    return EOF;

  return conversions;
}

} // namespace scanf_core
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_STDIO_SCANF_CORE_SCANF_MAIN_H
