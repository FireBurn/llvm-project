//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of struct mq_attr.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_MQ_ATTR_H
#define LLVM_LIBC_TYPES_STRUCT_MQ_ATTR_H

// What a queue was made with and how full it is. Only mq_flags may be
// changed once the queue exists; the rest are fixed at creation.
struct mq_attr {
  long mq_flags;   // O_NONBLOCK, or zero.
  long mq_maxmsg;  // How many messages it may hold.
  long mq_msgsize; // How large one may be.
  long mq_curmsgs; // How many are in it.
  long __pad[4];
};

#endif // LLVM_LIBC_TYPES_STRUCT_MQ_ATTR_H
