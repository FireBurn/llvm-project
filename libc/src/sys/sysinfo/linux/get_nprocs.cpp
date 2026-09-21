//===-- Linux implementation of get_nprocs --------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/sys/sysinfo/get_nprocs.h"

#include "hdr/types/cpu_set_t.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/sched/sched_getaffinity.h"
#include "src/sched/sched_getcpucount.h"

namespace LIBC_NAMESPACE_DECL {

// How many processors this process may actually run on, which is what
// callers sizing a thread pool want. An affinity mask narrower than the
// machine is the answer, not a reason to ignore it.
LLVM_LIBC_FUNCTION(int, get_nprocs, (void)) {
  cpu_set_t mask;
  if (sched_getaffinity(0, sizeof(mask), &mask) != 0)
    return 1;
  int count = __sched_getcpucount(sizeof(mask), &mask);
  return count > 0 ? count : 1;
}

} // namespace LIBC_NAMESPACE_DECL
