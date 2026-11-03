//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of arc4random_buf.
///
//===----------------------------------------------------------------------===//

#include "src/stdlib/arc4random_buf.h"
#include "hdr/types/size_t.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/stdlib/linux/arc4random_utils.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(void, arc4random_buf, (void *buf, size_t nbytes)) {
  arc4random_utils::fill(buf, nbytes);
}

} // namespace LIBC_NAMESPACE_DECL
