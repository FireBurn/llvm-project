//===-- Implementation of inet_makeaddr function --------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/arpa/inet/inet_makeaddr.h"
#include "hdr/types/in_addr_t.h"
#include "hdr/types/struct_in_addr.h"
#include "src/__support/common.h"
#include "src/__support/endian_internal.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(struct in_addr, inet_makeaddr,
                   (in_addr_t net, in_addr_t host)) {
  // How far the network number reaches tells which class of address it came
  // from, and so how many of the low bits are left for the host.
  in_addr_t address;
  if (net < 0x80)
    address = (net << 24) | (host & 0xffffff);
  else if (net < 0x10000)
    address = (net << 16) | (host & 0xffff);
  else if (net < 0x1000000)
    address = (net << 8) | (host & 0xff);
  else
    address = net | host;

  struct in_addr result;
  result.s_addr = Endian::to_big_endian(address);
  return result;
}

} // namespace LIBC_NAMESPACE_DECL
