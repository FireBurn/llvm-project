//===-- Implementation of gai_strerror ------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/netdb/gai_strerror.h"

#include "hdr/netdb_macros.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(const char *, gai_strerror, (int ecode)) {
  switch (ecode) {
  case 0:
    return "Success";
  case EAI_BADFLAGS:
    return "Bad value for ai_flags";
  case EAI_NONAME:
    return "Name or service not known";
  case EAI_AGAIN:
    return "Temporary failure in name resolution";
  case EAI_FAIL:
    return "Non-recoverable failure in name resolution";
  case EAI_FAMILY:
    return "ai_family not supported";
  case EAI_SOCKTYPE:
    return "ai_socktype not supported";
  case EAI_SERVICE:
    return "Servname not supported for ai_socktype";
  case EAI_MEMORY:
    return "Memory allocation failure";
  case EAI_SYSTEM:
    return "System error";
  case EAI_OVERFLOW:
    return "Argument buffer overflow";
  default:
    return "Unknown error";
  }
}

} // namespace LIBC_NAMESPACE_DECL
