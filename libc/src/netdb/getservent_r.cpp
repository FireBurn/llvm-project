//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of getservent_r
///
//===----------------------------------------------------------------------===//

#include "src/netdb/getservent_r.h"

#include "hdr/errno_macros.h"
#include "hdr/types/size_t.h"
#include "hdr/types/struct_servent.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/netdb/reentrant.h"
#include "src/netdb/serv_storage.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, getservent_r,
                   (struct servent * result_buf, char *buf, size_t buflen,
                    struct servent **result)) {
  if (result == nullptr)
    return EINVAL;
  *result = nullptr;
  if (result_buf == nullptr || buf == nullptr)
    return EINVAL;

  struct servent *found = resolv::next_service(internal::serv_storage());
  if (found == nullptr)
    return 0;

  netdb::Arena arena(buf, buflen);
  if (!netdb::copy_entry(arena, *found, result_buf))
    return ERANGE;
  *result = result_buf;
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL
