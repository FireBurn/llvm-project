//===-- Looking a service up by name ----------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_NETDB_RESOLV_SERVICES_H
#define LLVM_LIBC_SRC_NETDB_RESOLV_SERVICES_H

#include "hdr/stdint_proxy.h"
#include "hdr/types/size_t.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {
namespace resolv {

// The port `name` is registered on for `protocol`, in the order the wire
// uses. `protocol` is "tcp" or "udp", or null for either, in which case
// `found_protocol` is filled in with the one the file gave. Returns false if
// the file says nothing about the service.
bool port_for_service(const char *name, const char *protocol, uint16_t &port,
                      char *found_protocol = nullptr,
                      size_t found_capacity = 0);

} // namespace resolv
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_NETDB_RESOLV_SERVICES_H
