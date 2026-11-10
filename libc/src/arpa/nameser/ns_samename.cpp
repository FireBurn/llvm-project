//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of ns_samename
///
//===----------------------------------------------------------------------===//

#include "src/arpa/nameser/ns_samename.h"

#include "hdr/errno_macros.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/arpa/nameser/ns_makecanon.h"
#include "src/arpa/nameser/ns_name_codec.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, ns_samename, (const char *a, const char *b)) {
  char ca[nameser::MAX_WIRE_NAME + 2];
  char cb[nameser::MAX_WIRE_NAME + 2];
  if (a == nullptr || b == nullptr || ns_makecanon(a, ca, sizeof(ca)) < 0 ||
      ns_makecanon(b, cb, sizeof(cb)) < 0)
    return -1;
  const char *x = ca;
  const char *y = cb;
  for (; *x != '\0' && *y != '\0'; ++x, ++y)
    if (nameser::lower(static_cast<unsigned char>(*x)) !=
        nameser::lower(static_cast<unsigned char>(*y)))
      return 0;
  return *x == *y ? 1 : 0;
}

} // namespace LIBC_NAMESPACE_DECL
