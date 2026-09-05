//===-- Implementation of the h_errno accessor ----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/netdb/__h_errno_location.h"

#include "src/__support/common.h"
#include "src/__support/macros/attributes.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

namespace {
// What the last name lookup failed with. It is per thread for the same
// reason errno is: two threads looking up names at once must not overwrite
// each other's answer.
LIBC_THREAD_LOCAL int thread_h_errno;
} // anonymous namespace

// <netdb.h> defines h_errno as *__h_errno_location(), the same shape errno
// has, so that it can be per thread and still be assigned to.
LLVM_LIBC_FUNCTION(int *, __h_errno_location, (void)) {
  return &thread_h_errno;
}

} // namespace LIBC_NAMESPACE_DECL
