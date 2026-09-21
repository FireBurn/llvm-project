//===-- Implementation header for __sched_cpucount --------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_COMPAT___SCHED_CPUCOUNT_H
#define LLVM_LIBC_SRC_COMPAT___SCHED_CPUCOUNT_H

#include "hdr/types/cpu_set_t.h"
#include "hdr/types/size_t.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

int __sched_cpucount(size_t cpuset_size, const cpu_set_t *mask);

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_COMPAT___SCHED_CPUCOUNT_H
