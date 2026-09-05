//===-- Implementation of explicit_bzero ----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/string/explicit_bzero.h"

#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/string/memory_utils/inline_bzero.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(void, explicit_bzero, (void *ptr, size_t count)) {
  inline_bzero(reinterpret_cast<char *>(ptr), count);

  // This is the whole point of the call: a plain bzero over storage which is
  // about to go out of scope is dead and the compiler may drop it, taking
  // the key or the password with it. The barrier says the memory has been
  // read by something the compiler cannot see, so the write has to happen.
  asm volatile("" : : "r"(ptr) : "memory");
}

} // namespace LIBC_NAMESPACE_DECL
