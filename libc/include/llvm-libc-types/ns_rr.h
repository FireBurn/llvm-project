//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of ns_rr, one record of a name server message.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_NS_RR_H
#define LLVM_LIBC_TYPES_NS_RR_H

#include "../llvm-libc-macros/stdint-macros.h"

/// A record as ns_parserr reads it. Its data is left in the message, which
/// has to outlive the record.
typedef struct __ns_rr {
  char name[1025]; ///< NS_MAXDNAME.
  uint16_t type;
  uint16_t rr_class;
  uint32_t ttl;
  uint16_t rdlength;
  const unsigned char *rdata;
} ns_rr;

#endif // LLVM_LIBC_TYPES_NS_RR_H
