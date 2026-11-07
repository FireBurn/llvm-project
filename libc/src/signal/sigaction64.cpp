//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of sigaction64.
///
//===----------------------------------------------------------------------===//

#include "src/signal/sigaction64.h"
#include "hdr/types/struct_sigaction.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/signal/sigaction.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, sigaction64,
                   (int signal, const struct sigaction64 *__restrict libc_new,
                    struct sigaction64 *__restrict libc_old)) {
  return sigaction(signal, reinterpret_cast<const struct sigaction *>(libc_new),
                   reinterpret_cast<struct sigaction *>(libc_old));
}

} // namespace LIBC_NAMESPACE_DECL
