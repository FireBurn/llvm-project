//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation header for android_update_LD_LIBRARY_PATH.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_ANDROID_ANDROID_UPDATE_LD_LIBRARY_PATH_H
#define LLVM_LIBC_SRC_ANDROID_ANDROID_UPDATE_LD_LIBRARY_PATH_H

#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

void android_update_LD_LIBRARY_PATH(const char *ld_library_path);

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_ANDROID_ANDROID_UPDATE_LD_LIBRARY_PATH_H
