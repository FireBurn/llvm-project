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
#include "hdr/types/struct_servent.h"
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

// Where a service entry and the strings it points at are kept, one per
// thread, since each lookup replaces what the last handed back.
struct ServStorage {
  static constexpr size_t MAX_ALIASES = 15;
  static constexpr size_t POOL_SIZE = 256;

  struct servent entry;
  char *aliases[MAX_ALIASES + 1];
  char pool[POOL_SIZE];
  size_t used;
};

// Fills `storage` from the line naming `name`, or carrying `port`, for
// `protocol` where one is named. Returns null where the file says nothing.
struct servent *serv_by_name(const char *name, const char *protocol,
                             ServStorage &storage);
struct servent *serv_by_port(int port, const char *protocol,
                             ServStorage &storage);

// Reads the entries one after another.
void rewind_services(bool stay_open);
void stop_services();
struct servent *next_service(ServStorage &storage);

} // namespace resolv
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_NETDB_RESOLV_SERVICES_H
