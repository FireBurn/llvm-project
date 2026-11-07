//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of android_create_namespace.
///
//===----------------------------------------------------------------------===//

#include "src/android/android_create_namespace.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/dlfcn/dl_internal.h"

namespace LIBC_NAMESPACE_DECL {

// Every object is looked up the one way, so there is no namespace to make.
LLVM_LIBC_FUNCTION(struct android_namespace_t *, android_create_namespace,
                   (const char *, const char *, const char *, uint64_t,
                    const char *, struct android_namespace_t *)) {
  dl::set_error("linker namespaces are not supported");
  return nullptr;
}

} // namespace LIBC_NAMESPACE_DECL
