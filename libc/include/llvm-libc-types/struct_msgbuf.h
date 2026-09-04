//===-- Definition of struct msgbuf ---------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_MSGBUF_H
#define LLVM_LIBC_TYPES_STRUCT_MSGBUF_H

// The shape a message has: a type the receiver can select on, and then the
// message itself. A caller declares its own structure of this shape with
// however much room it needs rather than using this one.
struct msgbuf {
  long mtype;
  char mtext[1];
};

#endif // LLVM_LIBC_TYPES_STRUCT_MSGBUF_H
