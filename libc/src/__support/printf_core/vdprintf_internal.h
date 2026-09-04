//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Formatting straight to a file descriptor, which is what dprintf does.
/// There is no FILE in the way, so nothing else the process writes to the
/// same descriptor is interleaved with it beyond what the writes themselves
/// decide.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC___SUPPORT_PRINTF_CORE_VDPRINTF_INTERNAL_H
#define LLVM_LIBC_SRC___SUPPORT_PRINTF_CORE_VDPRINTF_INTERNAL_H

#include "src/__support/CPP/string_view.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/write.h"
#include "src/__support/arg_list.h"
#include "src/__support/error_or.h"
#include "src/__support/macros/config.h"
#include "src/__support/printf_core/core_structs.h"
#include "src/__support/printf_core/printf_main.h"
#include "src/__support/printf_core/writer.h"

namespace LIBC_NAMESPACE_DECL {
namespace printf_core {

constexpr size_t DPRINTF_BUFFER_SIZE = 1024;

LIBC_INLINE int write_to_descriptor(cpp::string_view new_str, void *target) {
  int fd = static_cast<int>(reinterpret_cast<size_t>(target));
  size_t written = 0;
  // A write is allowed to take less than it was given, so it is repeated
  // until the whole piece is out or it stops making progress.
  while (written < new_str.size()) {
    auto result = linux_syscalls::write(fd, new_str.data() + written,
                                        new_str.size() - written);
    if (!result.has_value())
      return FILE_WRITE_ERROR;
    if (result.value() == 0)
      return FILE_WRITE_ERROR;
    written += static_cast<size_t>(result.value());
  }
  return WRITE_OK;
}

LIBC_INLINE ErrorOr<size_t> vdprintf_internal(int fd,
                                              const char *__restrict format,
                                              internal::ArgList args) {
  char buffer[DPRINTF_BUFFER_SIZE];
  printf_core::FlushingBuffer wb(
      buffer, DPRINTF_BUFFER_SIZE, write_to_descriptor,
      reinterpret_cast<void *>(static_cast<size_t>(fd)));
  printf_core::Writer writer(wb);

  auto written = printf_core::printf_main(&writer, format, args);
  if (!written.has_value())
    return Error(written.error());
  int flushed = wb.flush_to_stream();
  if (flushed < 0)
    return Error(-flushed);
  return written.value();
}

} // namespace printf_core
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC___SUPPORT_PRINTF_CORE_VDPRINTF_INTERNAL_H
