//===-- Implementation of secure_getenv -----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/stdlib/secure_getenv.h"

#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/stdlib/environ_internal.h"
#include "src/sys/auxv/getauxval.h"

#include "hdr/sys_auxv_macros.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(char *, secure_getenv, (const char *name)) {
  if (name == nullptr || name[0] == '\0')
    return nullptr;

  // The kernel sets AT_SECURE when the program got privileges it did not
  // have before the exec, which is exactly when the environment is not to
  // be trusted.
  if (LIBC_NAMESPACE::getauxval(AT_SECURE) != 0)
    return nullptr;

  return internal::EnvironmentManager::get_instance().get(name);
}

} // namespace LIBC_NAMESPACE_DECL
