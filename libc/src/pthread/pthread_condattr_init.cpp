//===-- Implementation of the pthread_condattr_init -----------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "pthread_condattr_init.h"

#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/pthread/pthread_condattr_bits.h"

#include "hdr/time_macros.h" // CLOCK_REALTIME
#include <pthread.h>         // pthread_condattr_t, PTHREAD_PROCESS_PRIVATE

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, pthread_condattr_init, (pthread_condattr_t * attr)) {
  condattr::set_clock(*attr, CLOCK_REALTIME);
  condattr::set_pshared(*attr, PTHREAD_PROCESS_PRIVATE);
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL
