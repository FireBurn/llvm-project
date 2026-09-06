//===-- Linux implementation of freeifaddrs -------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/ifaddrs/freeifaddrs.h"

#include "hdr/func/free.h"
#include "hdr/types/struct_ifaddrs.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

// The whole list is one allocation which starts at the head, so there is one
// thing to give back however many entries there were.
LLVM_LIBC_FUNCTION(void, freeifaddrs, (struct ifaddrs * list)) { free(list); }

} // namespace LIBC_NAMESPACE_DECL
