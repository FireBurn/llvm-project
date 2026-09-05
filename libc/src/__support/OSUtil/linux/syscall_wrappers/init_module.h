//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Syscall wrapper for init_module.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC___SUPPORT_OSUTIL_SYSCALL_WRAPPERS_INIT_MODULE_H
#define LLVM_LIBC_SRC___SUPPORT_OSUTIL_SYSCALL_WRAPPERS_INIT_MODULE_H

#include "src/__support/OSUtil/linux/syscall.h" // syscall_checked
#include "src/__support/common.h"
#include "src/__support/error_or.h"
#include "src/__support/macros/config.h"
#include <sys/syscall.h> // For syscall numbers

namespace LIBC_NAMESPACE_DECL {
namespace linux_syscalls {

LIBC_INLINE ErrorOr<long> init_module(const void *module_image,
                                      unsigned long len,
                                      const char *param_values) {
  return syscall_checked<long>(SYS_init_module, module_image, len,
                               param_values);
}

} // namespace linux_syscalls
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC___SUPPORT_OSUTIL_SYSCALL_WRAPPERS_INIT_MODULE_H
