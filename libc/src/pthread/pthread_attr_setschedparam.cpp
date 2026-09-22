//===-- Implementation of pthread_attr_setschedparam ----------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "pthread_attr_setschedparam.h"

#include "hdr/errno_macros.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"

#include <pthread.h>
#include <sched.h>

namespace LIBC_NAMESPACE_DECL {

// Whether the priority is one this process may ask for depends on the
// policy it goes with, so it is the kernel that says, at creation.
LLVM_LIBC_FUNCTION(int, pthread_attr_setschedparam,
                   (pthread_attr_t *__restrict attr,
                    const struct sched_param *__restrict param)) {
  if (attr == nullptr || param == nullptr)
    return EINVAL;
  attr->__schedparam = *param;
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL
