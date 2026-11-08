//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
///
/// \file
/// Linux implementation of the pthread_setschedprio function.
///
//===----------------------------------------------------------------------===//

#include "pthread_setschedprio.h"

#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/__support/threads/thread.h"

namespace LIBC_NAMESPACE_DECL {

static_assert(sizeof(pthread_t) == sizeof(LIBC_NAMESPACE::Thread),
              "Mismatch between pthread_t and internal Thread.");

LLVM_LIBC_FUNCTION(int, pthread_setschedprio, (pthread_t th, int prio)) {
  auto *thread = reinterpret_cast<Thread *>(&th);
  return thread->setschedprio(prio);
}

} // namespace LIBC_NAMESPACE_DECL
