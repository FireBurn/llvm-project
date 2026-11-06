//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of pthread_barrierattr_init.
///
//===----------------------------------------------------------------------===//

#include "pthread_barrierattr_init.h"

#include "src/__support/common.h"
#include "src/__support/macros/config.h"

#include <pthread.h>

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, pthread_barrierattr_init,
                   (pthread_barrierattr_t * attr)) {
  attr->pshared = PTHREAD_PROCESS_PRIVATE;
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL
