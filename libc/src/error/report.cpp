//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of the message error and error_at_line write.
///
//===----------------------------------------------------------------------===//

#include "src/error/report.h"

#include "hdr/types/FILE.h"
#include "src/__support/CPP/string_view.h"
#include "src/__support/File/file.h"
#include "src/__support/OSUtil/io.h"
#include "src/__support/StringUtil/error_to_string.h"
#include "src/__support/arg_list.h"
#include "src/__support/macros/attributes.h"
#include "src/__support/macros/config.h"
#include "src/__support/printf_core/printf_main.h"
#include "src/__support/printf_core/writer.h"
#include "src/error/error_message_count.h"
#include "src/error/error_print_progname.h"

#ifdef LIBC_FULL_BUILD
#include "src/errno/program_invocation_name.h"
#define PROGRAM_INVOCATION_NAME LIBC_NAMESPACE::program_invocation_name
#else
extern "C" char *program_invocation_name;
#define PROGRAM_INVOCATION_NAME ::program_invocation_name
#endif

namespace LIBC_NAMESPACE_DECL {

// Anything the program has written and not yet handed over goes out before
// the message does, so that the two arrive in the order they were written in.
// The stream is referred to weakly: a program which never uses it does not
// pull it in for the sake of this.
extern FILE *stdout [[gnu::weak]];

namespace error_reporting {

namespace {

LIBC_INLINE void flush_stdout() {
  FILE *const *slot = &stdout;
  if (slot == nullptr || *slot == nullptr)
    return;
  reinterpret_cast<File *>(*slot)->flush();
}

LIBC_INLINE int stderr_sink(cpp::string_view str, void *) {
  write_to_stderr(str);
  return printf_core::WRITE_OK;
}

template <typename W>
LIBC_INLINE void write_unsigned(W &writer, unsigned int val) {
  char digits[10];
  size_t len = 0;
  do {
    digits[len++] = static_cast<char>('0' + val % 10);
    val /= 10;
  } while (val != 0);
  char out[sizeof(digits)];
  for (size_t i = 0; i < len; ++i)
    out[i] = digits[len - 1 - i];
  writer.write(cpp::string_view(out, len));
}

} // anonymous namespace

void report(int err_num, const char *file_name, unsigned int line_number,
            bool with_location, const char *fmt, internal::ArgList &args) {
  flush_stdout();

  char buffer[1024];
  printf_core::Writer writer = printf_core::make_writer(
      buffer, sizeof(buffer),
      &printf_core::overflow_write_flush_to_sink<char, stderr_sink>);

  if (error_print_progname != nullptr) {
    error_print_progname();
  } else {
    const char *progname = PROGRAM_INVOCATION_NAME;
    writer.write(progname == nullptr ? "" : progname);
    writer.write(with_location ? ":" : ": ");
  }

  if (with_location) {
    if (file_name != nullptr) {
      writer.write(file_name);
      writer.write(":");
      write_unsigned(writer, line_number);
      writer.write(": ");
    } else {
      writer.write(" ");
    }
  }

  if (fmt != nullptr && !printf_core::printf_main(&writer, fmt, args)) {
    writer.get_write_buffer().flush_to_sink<stderr_sink>();
    return;
  }

  if (err_num != 0) {
    writer.write(": ");
    writer.write(get_error_string(err_num));
  }

  writer.write("\n");
  writer.get_write_buffer().flush_to_sink<stderr_sink>();

  ++error_message_count;
}

} // namespace error_reporting
} // namespace LIBC_NAMESPACE_DECL
