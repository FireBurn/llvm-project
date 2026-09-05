//===-- Implementation of putspent ----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/shadow/putspent.h"

#include "hdr/errno_macros.h"
#include "hdr/types/FILE.h"
#include "hdr/types/struct_spwd.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/stdio/fprintf.h"

namespace LIBC_NAMESPACE_DECL {

namespace {

// A day count of -1 means the file leaves that field empty.
int print_count(::FILE *stream, long value) {
  if (value < 0)
    return LIBC_NAMESPACE::fprintf(stream, ":");
  return LIBC_NAMESPACE::fprintf(stream, "%ld:", value);
}

} // anonymous namespace

LLVM_LIBC_FUNCTION(int, putspent, (const struct spwd *p, ::FILE *stream)) {
  if (p == nullptr || stream == nullptr || p->sp_namp == nullptr) {
    libc_errno = EINVAL;
    return -1;
  }

  if (LIBC_NAMESPACE::fprintf(stream, "%s:%s:", p->sp_namp,
                              p->sp_pwdp != nullptr ? p->sp_pwdp : "") < 0)
    return -1;
  if (print_count(stream, p->sp_lstchg) < 0 ||
      print_count(stream, p->sp_min) < 0 ||
      print_count(stream, p->sp_max) < 0 ||
      print_count(stream, p->sp_warn) < 0 ||
      print_count(stream, p->sp_inact) < 0 ||
      print_count(stream, p->sp_expire) < 0)
    return -1;

  // The last field carries no separator after it, and an unset flag is
  // written as nothing rather than as a number.
  if (p->sp_flag == static_cast<unsigned long>(-1)) {
    if (LIBC_NAMESPACE::fprintf(stream, "\n") < 0)
      return -1;
  } else if (LIBC_NAMESPACE::fprintf(stream, "%lu\n", p->sp_flag) < 0) {
    return -1;
  }
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL
