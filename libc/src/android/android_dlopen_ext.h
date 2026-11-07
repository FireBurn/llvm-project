//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation header for android_dlopen_ext.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_ANDROID_ANDROID_DLOPEN_EXT_H
#define LLVM_LIBC_SRC_ANDROID_ANDROID_DLOPEN_EXT_H

#include "hdr/types/android_dlextinfo.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

void *android_dlopen_ext(const char *filename, int flags,
                         const android_dlextinfo *extinfo);

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_ANDROID_ANDROID_DLOPEN_EXT_H
