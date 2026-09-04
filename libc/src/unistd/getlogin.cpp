//===-- Implementation of getlogin ----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/unistd/getlogin.h"

#include "hdr/limits_macros.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/unistd/getlogin_r.h"

namespace LIBC_NAMESPACE_DECL {

namespace {

// getlogin is not reentrant: the name it returns is in a buffer shared by
// every caller in the process.
char name[LOGIN_NAME_MAX];

} // anonymous namespace

LLVM_LIBC_FUNCTION(char *, getlogin, (void)) {
  int err = LIBC_NAMESPACE::getlogin_r(name, sizeof(name));
  if (err != 0) {
    libc_errno = err;
    return nullptr;
  }
  return name;
}

} // namespace LIBC_NAMESPACE_DECL
