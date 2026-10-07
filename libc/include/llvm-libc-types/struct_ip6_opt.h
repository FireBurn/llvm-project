//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of struct ip6_opt.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_IP6_OPT_H
#define LLVM_LIBC_TYPES_STRUCT_IP6_OPT_H

#include "../llvm-libc-macros/stdint-macros.h"
// The start of an IPv6 option.
struct ip6_opt {
  uint8_t ip6o_type;
  uint8_t ip6o_len;
};

#endif // LLVM_LIBC_TYPES_STRUCT_IP6_OPT_H
