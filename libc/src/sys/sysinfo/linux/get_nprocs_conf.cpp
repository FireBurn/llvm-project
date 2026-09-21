//===-- Linux implementation of get_nprocs_conf ---------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/sys/sysinfo/get_nprocs_conf.h"

#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/sys/sysinfo/get_nprocs.h"

namespace LIBC_NAMESPACE_DECL {

// What the machine is configured with rather than what this process may
// use. Reading that apart from the affinity mask means parsing sysfs, so
// until something needs the distinction this reports the same number.
LLVM_LIBC_FUNCTION(int, get_nprocs_conf, (void)) { return get_nprocs(); }

} // namespace LIBC_NAMESPACE_DECL
