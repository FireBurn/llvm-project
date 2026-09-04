//===-- Implementation of getopt ------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/unistd/getopt.h"
#include "src/__support/CPP/optional.h"
#include "src/__support/CPP/string_view.h"
#include "src/__support/File/file.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/stdio/fprintf.h"
#include "src/stdio/stderr.h"
#include "src/unistd/getopt_common.h"

#include "hdr/types/FILE.h"

// This is POSIX compliant and does not support GNU extensions, mainly this is
// just the re-ordering of argv elements such that unknown arguments can be
// easily iterated over.

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_VARIABLE(char *, optarg) = nullptr;
LLVM_LIBC_VARIABLE(int, optind) = 1;
LLVM_LIBC_VARIABLE(int, optopt) = 0;
LLVM_LIBC_VARIABLE(int, opterr) = 0;

namespace impl {

static unsigned optpos;

GetoptContext ctx{&optarg, &optind, &optopt,
                  &optpos, &opterr, /*errstream=*/nullptr};

#ifndef LIBC_COPT_PUBLIC_PACKAGING
// This is used exclusively in tests.
void set_getopt_state(char **optarg_in, int *optind_in, int *optopt_in,
                      unsigned *optpos_in, int *opterr_in, FILE *errstream) {
  ctx = {optarg_in, optind_in, optopt_in, optpos_in, opterr_in, errstream};
}
#endif

} // namespace impl

LLVM_LIBC_FUNCTION(int, getopt,
                   (int argc, char *const argv[], const char *optstring)) {
  return getopt_r(argc, argv, optstring, impl::ctx);
}

} // namespace LIBC_NAMESPACE_DECL
