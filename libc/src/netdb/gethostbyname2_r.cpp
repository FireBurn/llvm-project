//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of gethostbyname2_r
///
//===----------------------------------------------------------------------===//

#include "src/netdb/gethostbyname2_r.h"

#include "hdr/errno_macros.h"
#include "hdr/netdb_macros.h"
#include "hdr/types/size_t.h"
#include "hdr/types/struct_hostent.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/netdb/__h_errno_location.h"
#include "src/netdb/hostent_storage.h"
#include "src/netdb/hosts_storage.h"
#include "src/netdb/reentrant.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, gethostbyname2_r,
                   (const char *name, int af, struct hostent *result_buf,
                    char *buf, size_t buflen, struct hostent **result,
                    int *h_errnop)) {
  if (result == nullptr)
    return EINVAL;
  *result = nullptr;
  if (result_buf == nullptr || buf == nullptr)
    return EINVAL;

  struct hostent *found = netdb::fill_hostent(name, af);
  if (found == nullptr) {
    int error = *internal::h_errno_location_impl();
    if (h_errnop != nullptr)
      *h_errnop = error;
    return error == TRY_AGAIN ? EAGAIN : ENOENT;
  }

  netdb::Arena arena(buf, buflen);
  if (!netdb::copy_entry(arena, *found, result_buf))
    return ERANGE;
  if (h_errnop != nullptr)
    *h_errnop = 0;
  *result = result_buf;
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL
