//===-- Implementation of ctermid -----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/stdio/ctermid.h"
#include "hdr/stdio_macros.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/string/memory_utils/inline_memcpy.h"

namespace LIBC_NAMESPACE_DECL {

namespace {
// Every process on Linux reaches its controlling terminal by the same name,
// so there is nothing to look up.
constexpr char TERMINAL_NAME[] = "/dev/tty";
// Returned where the caller passed no buffer of its own. POSIX says the array
// this points at may be overwritten by a later call.
char terminal_name_buffer[L_ctermid];
} // anonymous namespace

LLVM_LIBC_FUNCTION(char *, ctermid, (char *s)) {
  char *dest = s == nullptr ? terminal_name_buffer : s;
  inline_memcpy(dest, TERMINAL_NAME, sizeof(TERMINAL_NAME));
  return dest;
}

} // namespace LIBC_NAMESPACE_DECL
