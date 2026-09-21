//===-- Implementation of getservbyport_r ---------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/netdb/getservbyport_r.h"

#include "hdr/errno_macros.h"
#include "hdr/types/size_t.h"
#include "hdr/types/struct_servent.h"
#include "src/__support/CPP/new.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/netdb/resolv/services.h"

namespace LIBC_NAMESPACE_DECL {

// The reentrant form: the entry and every string it points at live in the
// caller's buffer, so two threads can look up at once. The buffer has to
// hold the whole of one of these, which is what ERANGE reports when it
// does not.
LLVM_LIBC_FUNCTION(int, getservbyport_r,
                   (int port, const char *proto,
                    struct servent *result_buf, char *buf, size_t buflen,
                    struct servent **result)) {
  if (result == nullptr)
    return EINVAL;
  *result = nullptr;
  if (result_buf == nullptr || buf == nullptr)
    return EINVAL;

  // The strings have to be reachable after this returns, so they go in the
  // caller's buffer rather than anywhere this function owns.
  if (buflen < sizeof(resolv::ServStorage) + alignof(resolv::ServStorage))
    return ERANGE;

  void *aligned = buf;
  size_t offset = reinterpret_cast<size_t>(buf) % alignof(resolv::ServStorage);
  if (offset != 0)
    aligned = buf + (alignof(resolv::ServStorage) - offset);

  auto *storage = new (aligned) resolv::ServStorage();
  struct servent *found = resolv::serv_by_port(port, proto, *storage);
  if (found == nullptr)
    return 0;

  *result_buf = *found;
  *result = result_buf;
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL
