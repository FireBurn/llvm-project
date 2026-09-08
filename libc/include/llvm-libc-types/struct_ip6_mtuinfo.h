//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of struct ip6_mtuinfo.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_IP6_MTUINFO_H
#define LLVM_LIBC_TYPES_STRUCT_IP6_MTUINFO_H


#include "../llvm-libc-macros/stdint-macros.h"
#include "struct_sockaddr_in6.h"

// What IPV6_PATHMTU reports: the path's largest packet, in host order,
// and where that path leads.
struct ip6_mtuinfo {
  struct sockaddr_in6 ip6m_addr;
  uint32_t ip6m_mtu;
};

#endif // LLVM_LIBC_TYPES_STRUCT_IP6_MTUINFO_H