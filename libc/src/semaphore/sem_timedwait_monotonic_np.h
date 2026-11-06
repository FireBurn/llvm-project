//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation header for sem_timedwait_monotonic_np.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_SEMAPHORE_SEM_TIMEDWAIT_MONOTONIC_NP_H
#define LLVM_LIBC_SRC_SEMAPHORE_SEM_TIMEDWAIT_MONOTONIC_NP_H

#include "hdr/types/sem_t.h"
#include "hdr/types/struct_timespec.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

int sem_timedwait_monotonic_np(sem_t *__restrict sem,
                               const struct timespec *__restrict abstime);

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_SEMAPHORE_SEM_TIMEDWAIT_MONOTONIC_NP_H
