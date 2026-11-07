//===-- Definition of struct iovec ----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_IOVEC_H
#define LLVM_LIBC_TYPES_STRUCT_IOVEC_H

#include "size_t.h"

// Android programs include <linux/uio.h> for the same structure, as bionic's
// own header does, and a second definition would not compile.
#if defined(__ANDROID__) && defined(__has_include)
#if __has_include(<linux/uio.h>)
#include <linux/uio.h>
#define LIBC_IOVEC_FROM_KERNEL
#endif
#endif

#ifndef LIBC_IOVEC_FROM_KERNEL
struct iovec {
  void *iov_base;
  size_t iov_len;
};
#endif

#endif // LLVM_LIBC_TYPES_STRUCT_IOVEC_H
