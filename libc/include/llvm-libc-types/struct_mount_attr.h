//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of struct mount_attr.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_MOUNT_ATTR_H
#define LLVM_LIBC_TYPES_STRUCT_MOUNT_ATTR_H

#include "../llvm-libc-macros/stdint-macros.h"

// What mount_setattr is to change: the attributes to turn on, the ones to
// turn off, what to set the propagation to, and the user namespace an
// idmapped mount is to map through.
struct mount_attr {
  uint64_t attr_set;
  uint64_t attr_clr;
  uint64_t propagation;
  uint64_t userns_fd;
};

#endif // LLVM_LIBC_TYPES_STRUCT_MOUNT_ATTR_H
