//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation header for android_create_namespace.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_ANDROID_ANDROID_CREATE_NAMESPACE_H
#define LLVM_LIBC_SRC_ANDROID_ANDROID_CREATE_NAMESPACE_H

#include "hdr/stdint_proxy.h"
#include "hdr/types/android_dlextinfo.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

struct android_namespace_t *
android_create_namespace(const char *name, const char *ld_library_path,
                         const char *default_library_path, uint64_t type,
                         const char *permitted_when_isolated_path,
                         struct android_namespace_t *parent);

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_ANDROID_ANDROID_CREATE_NAMESPACE_H
