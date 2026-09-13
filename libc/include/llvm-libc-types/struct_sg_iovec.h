//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of struct sg_iovec.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_SG_IOVEC_H
#define LLVM_LIBC_TYPES_STRUCT_SG_IOVEC_H

#include "size_t.h"

// One piece of a scattered buffer, laid out as struct iovec is.
typedef struct sg_iovec {
  void *iov_base;
  size_t iov_len;
} sg_iovec_t;

#endif // LLVM_LIBC_TYPES_STRUCT_SG_IOVEC_H
