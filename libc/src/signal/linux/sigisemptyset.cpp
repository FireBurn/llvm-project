//===-- Linux implementation of sigisemptyset -----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/signal/sigisemptyset.h"

#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/signal/linux/signal_utils.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, sigisemptyset, (const sigset_t *set)) {
  if (!set) {
    libc_errno = EINVAL;
    return -1;
  }
  for (size_t i = 0; i < sizeof(set->__signals) / sizeof(set->__signals[0]);
       ++i)
    if (set->__signals[i] != 0)
      return 0;
  return 1;
}

} // namespace LIBC_NAMESPACE_DECL
