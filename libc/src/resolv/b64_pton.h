//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation header for b64_pton
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_RESOLV_B64_PTON_H
#define LLVM_LIBC_SRC_RESOLV_B64_PTON_H

#include "src/__support/macros/config.h"

#include <stddef.h>

namespace LIBC_NAMESPACE_DECL {

int b64_pton(const char *src, unsigned char *target, size_t targsize);

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_RESOLV_B64_PTON_H
