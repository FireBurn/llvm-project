//===-- Implementation of herror ------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/netdb/herror.h"

#include "src/__support/CPP/string_view.h"
#include "src/__support/OSUtil/io.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/netdb/__h_errno_location.h"
#include "src/netdb/hstrerror.h"

namespace LIBC_NAMESPACE_DECL {

// Says what the last name lookup went wrong with, after whatever the caller
// wants to say first.
LLVM_LIBC_FUNCTION(void, herror, (const char *s)) {
  if (s != nullptr && *s != '\0') {
    write_to_stderr(s);
    write_to_stderr(": ");
  }
  write_to_stderr(hstrerror(*__h_errno_location()));
  write_to_stderr("\n");
}

} // namespace LIBC_NAMESPACE_DECL
