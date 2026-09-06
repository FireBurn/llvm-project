//===-- Implementation of res_init ----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/compat/res_init.h"

#include "src/__support/common.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

// Callers ask for this to make the resolver read what it was told about name
// servers again, after the file saying so has changed. Here every lookup
// reads that file as it goes, so there is nothing held over to throw away and
// nothing to do. Answering that it worked is the truth: the next lookup does
// use what the file now says.
//
// Rust's standard library calls it after a lookup fails, which is how it came
// to be needed here.
LLVM_LIBC_FUNCTION(int, res_init, (void)) { return 0; }

LLVM_LIBC_FUNCTION(int, __res_init, (void)) { return 0; }

} // namespace LIBC_NAMESPACE_DECL
