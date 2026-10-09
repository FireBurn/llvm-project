//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of pthread_getattr_default_np
///
//===----------------------------------------------------------------------===//

#include "pthread_getattr_default_np.h"

#include "hdr/errno_macros.h"
#include "hdr/pthread_macros.h"
#include "hdr/types/pthread_attr_t.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/pthread/pthread_default_attr.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, pthread_getattr_default_np, (pthread_attr_t * attr)) {
  if (attr == nullptr)
    return EINVAL;
  *attr = default_thread_attr();
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL
