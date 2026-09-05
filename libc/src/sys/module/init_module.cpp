//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Linux implementation of init_module
///
//===----------------------------------------------------------------------===//

#include "src/sys/module/init_module.h"

#include "src/__support/OSUtil/linux/syscall_wrappers/init_module.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(long, init_module,
                   (const void *module_image, unsigned long len,
                    const char *param_values)) {
  ErrorOr<long> ret =
      linux_syscalls::init_module(module_image, len, param_values);
  if (!ret) {
    libc_errno = ret.error();
    return -1;
  }
  return *ret;
}

} // namespace LIBC_NAMESPACE_DECL
