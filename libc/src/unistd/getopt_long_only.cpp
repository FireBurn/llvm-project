//===-- Implementation of
// getopt_long_only------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/unistd/getopt_long_only.h"

#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/unistd/getopt_long_impl.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, getopt_long_only,
                   (int argc, char *const argv[], const char *optstring,
                    const struct option *longopts, int *longindex)) {
  return internal::getopt_long_r(argc, argv, optstring, longopts, longindex,
                                 /*long_only=*/true, impl::ctx);
}

} // namespace LIBC_NAMESPACE_DECL
