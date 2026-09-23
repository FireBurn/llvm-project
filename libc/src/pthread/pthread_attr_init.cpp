//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of pthread_attr_init.
///
//===----------------------------------------------------------------------===//

#include "src/pthread/pthread_attr_init.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/__support/macros/null_check.h"
#include "src/pthread/pthread_attr.h"

namespace LIBC_NAMESPACE_DECL {

static_assert(sizeof(pthread_attr_t) == 56,
              "pthread_attr_t must be the size glibc gives it");

LLVM_LIBC_FUNCTION(int, pthread_attr_init, (pthread_attr_t * attr)) {
  LIBC_CRASH_ON_NULLPTR(attr);

  *attr = DEFAULT_PTHREAD_ATTR;
  // The stack a thread is given by default is the one the process is allowed,
  // as with glibc, rather than a fixed size.
  attr->__stacksize = Thread::default_stacksize();
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL
