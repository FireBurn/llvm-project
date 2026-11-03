//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the definition of enum nss_status.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_ENUM_NSS_STATUS_H
#define LLVM_LIBC_TYPES_ENUM_NSS_STATUS_H

/// What a service module says about a lookup.
enum nss_status {
  NSS_STATUS_TRYAGAIN = -2,
  NSS_STATUS_UNAVAIL = -1,
  NSS_STATUS_NOTFOUND = 0,
  NSS_STATUS_SUCCESS = 1,
  NSS_STATUS_RETURN = 2
};

#endif // LLVM_LIBC_TYPES_ENUM_NSS_STATUS_H
