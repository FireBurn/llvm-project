//===-- Implementation of __sched_cpucount --------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/compat/__sched_cpucount.h"

#include "hdr/types/cpu_set_t.h"
#include "hdr/types/size_t.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/sched/sched_getcpucount.h"

namespace LIBC_NAMESPACE_DECL {

// The name glibc puts behind CPU_COUNT. This library spells the same thing
// __sched_getcpucount, so code built against glibc asks for a name that is
// otherwise not here.
LLVM_LIBC_FUNCTION(int, __sched_cpucount,
                   (size_t cpuset_size, const cpu_set_t *mask)) {
  return __sched_getcpucount(cpuset_size, mask);
}

} // namespace LIBC_NAMESPACE_DECL
