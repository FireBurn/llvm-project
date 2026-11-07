//===-- Linux implementation of read --------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of __read_chk.
///
//===----------------------------------------------------------------------===//

#include "src/unistd/read_chk.h"
#include "src/__support/CPP/string_view.h"
#include "src/__support/abort_message.h"
#include "src/__support/common.h"
#include "src/__support/integer_to_string.h"
#include "src/__support/macros/config.h"
#include "src/__support/macros/optimization.h"
#include "src/stdlib/abort_utils.h"
#include "src/unistd/read.h"

namespace LIBC_NAMESPACE_DECL {

// What bionic's FORTIFY calls in place of read when the destination is known
// to be smaller than the count. The words of the message are bionic's, since
// debuggerd and the people reading a tombstone look for them.
LLVM_LIBC_FUNCTION(ssize_t, __read_chk,
                   (int fd, void *buf, size_t count, size_t buf_size)) {
  if (LIBC_UNLIKELY(count > buf_size)) {
    const IntegerToString<size_t> count_text(count);
    const IntegerToString<size_t> size_text(buf_size);
    char message[abort_message::CAPACITY];
    size_t used = 0;
    auto append = [&](cpp::string_view part) {
      for (char c : part)
        if (used < sizeof(message) - 1)
          message[used++] = c;
    };
    append("FORTIFY: read: prevented ");
    append(count_text.view());
    append("-byte write into ");
    append(size_text.view());
    append("-byte buffer");
    message[used] = '\0';
    abort_message::set(message);
    abort_utils::abort();
  }
  return LIBC_NAMESPACE::read(fd, buf, count);
}

} // namespace LIBC_NAMESPACE_DECL
