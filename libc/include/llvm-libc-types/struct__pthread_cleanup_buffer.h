//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_PTHREAD_CLEANUP_BUFFER_H
#define LLVM_LIBC_TYPES_STRUCT_PTHREAD_CLEANUP_BUFFER_H

/// One entry of a thread's stack of cleanup handlers. It lives in the frame
/// that pushed it, which is why pthread_cleanup_push and pthread_cleanup_pop
/// have to appear in the same block as each other.
struct _pthread_cleanup_buffer {
  void (*__routine)(void *);
  void *__arg;
  struct _pthread_cleanup_buffer *__prev;
  int __canceltype;
};

#endif // LLVM_LIBC_TYPES_STRUCT_PTHREAD_CLEANUP_BUFFER_H
