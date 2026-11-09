//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation header for android_getaddrinfofornet.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_ANDROID_ANDROID_GETADDRINFOFORNET_H
#define LLVM_LIBC_SRC_ANDROID_ANDROID_GETADDRINFOFORNET_H

#include "hdr/types/struct_addrinfo.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

int android_getaddrinfofornet(const char *nodename, const char *servname,
                              const struct addrinfo *hints, unsigned netid,
                              unsigned mark, struct addrinfo **res);

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_ANDROID_ANDROID_GETADDRINFOFORNET_H
