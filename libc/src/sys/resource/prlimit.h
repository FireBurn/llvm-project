//===-- Implementation header for prlimit -----------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_SYS_RESOURCE_PRLIMIT_H
#define LLVM_LIBC_SRC_SYS_RESOURCE_PRLIMIT_H

#include "hdr/types/pid_t.h"
#include "hdr/types/struct_rlimit.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

int prlimit(pid_t pid, int resource, const struct rlimit *new_limit,
            struct rlimit *old_limit);

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_SYS_RESOURCE_PRLIMIT_H
