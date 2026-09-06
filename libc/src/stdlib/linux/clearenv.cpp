//===-- Implementation of clearenv ----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/stdlib/clearenv.h"

#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/stdlib/environ_internal.h"

namespace LIBC_NAMESPACE_DECL {

// Empties the environment. A program about to run something else with an
// environment of its own builds it up from nothing this way.
LLVM_LIBC_FUNCTION(int, clearenv, (void)) {
  int result = internal::EnvironmentManager::get_instance().clear();
  if (result != 0)
    libc_errno = ENOMEM;
  return result;
}

} // namespace LIBC_NAMESPACE_DECL
