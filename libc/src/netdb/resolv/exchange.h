//===-- One exchange with a name server -------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_NETDB_RESOLV_EXCHANGE_H
#define LLVM_LIBC_SRC_NETDB_RESOLV_EXCHANGE_H

#include "hdr/types/size_t.h"
#include "src/__support/macros/config.h"
#include "src/netdb/resolv/resolv_conf.h"

namespace LIBC_NAMESPACE_DECL {
namespace resolv {

// Sends |message| to each server in |conf| in turn and copies the first reply
// into |answer|. Returns how long the reply is, or -1 where no server
// answered. The reply is handed over as it arrived, without being read: this
// is what res_send is, and what res_query is built from.
int exchange(const ResolvConf &conf, const unsigned char *message,
             size_t length, unsigned char *answer, size_t capacity);

} // namespace resolv
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_NETDB_RESOLV_EXCHANGE_H
