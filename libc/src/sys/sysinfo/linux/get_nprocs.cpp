//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Linux implementation of get_nprocs
///
//===----------------------------------------------------------------------===//

#include "src/sys/sysinfo/get_nprocs.h"

#include "hdr/types/cpu_set_t.h"
#include "src/__support/CPP/span.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/sched_getaffinity.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/sched/sched_getcpucount.h"

namespace LIBC_NAMESPACE_DECL {

namespace internal {

// How many processors this process may actually run on, which is what
// callers sizing a thread pool want. An affinity mask narrower than the
// machine is the answer, not a reason to ignore it.
int get_nprocs_impl() {
  cpu_set_t mask;
  if (!linux_syscalls::sched_getaffinity(
          0, cpp::span<unsigned char>(reinterpret_cast<unsigned char *>(&mask),
                                      sizeof(mask))))
    return 1;
  int count = internal::sched_getcpucount_impl(sizeof(mask), &mask);
  return count > 0 ? count : 1;
}

} // namespace internal

LLVM_LIBC_FUNCTION(int, get_nprocs, (void)) {
  return internal::get_nprocs_impl();
}

} // namespace LIBC_NAMESPACE_DECL
