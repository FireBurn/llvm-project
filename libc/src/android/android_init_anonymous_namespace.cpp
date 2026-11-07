//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of android_init_anonymous_namespace.
///
//===----------------------------------------------------------------------===//

#include "src/android/android_init_anonymous_namespace.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/dlfcn/dl_internal.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(bool, android_init_anonymous_namespace,
                   (const char *, const char *)) {
  dl::set_error("linker namespaces are not supported");
  return false;
}

} // namespace LIBC_NAMESPACE_DECL
