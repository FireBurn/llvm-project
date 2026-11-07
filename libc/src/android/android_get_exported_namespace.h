//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation header for android_get_exported_namespace.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_ANDROID_ANDROID_GET_EXPORTED_NAMESPACE_H
#define LLVM_LIBC_SRC_ANDROID_ANDROID_GET_EXPORTED_NAMESPACE_H

#include "hdr/types/android_dlextinfo.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

struct android_namespace_t *android_get_exported_namespace(const char *name);

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_ANDROID_ANDROID_GET_EXPORTED_NAMESPACE_H
