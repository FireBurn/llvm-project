//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation header for android_init_anonymous_namespace.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_ANDROID_ANDROID_INIT_ANONYMOUS_NAMESPACE_H
#define LLVM_LIBC_SRC_ANDROID_ANDROID_INIT_ANONYMOUS_NAMESPACE_H

#include "hdr/types/android_dlextinfo.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

bool android_init_anonymous_namespace(const char *shared_libs_sonames,
                                      const char *library_search_path);

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_ANDROID_ANDROID_INIT_ANONYMOUS_NAMESPACE_H
