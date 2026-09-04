//===-- Implementation header for fanotify_mark -----------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_SYS_FANOTIFY_FANOTIFY_MARK_H
#define LLVM_LIBC_SRC_SYS_FANOTIFY_FANOTIFY_MARK_H

#include "hdr/stdint_proxy.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

int fanotify_mark(int fanotify_fd, unsigned int flags, uint64_t mask, int dirfd,
                  const char *pathname);

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_SYS_FANOTIFY_FANOTIFY_MARK_H
