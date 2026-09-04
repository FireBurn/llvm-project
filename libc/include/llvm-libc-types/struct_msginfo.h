//===-- Definition of struct msginfo --------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_MSGINFO_H
#define LLVM_LIBC_TYPES_STRUCT_MSGINFO_H

// The limits msgctl reports for IPC_INFO, and, with the members standing for
// something else, the counts it reports for MSG_INFO.
struct msginfo {
  int msgpool; // How much memory is set aside for message queues.
  int msgmap;  // Entries in the message map.
  int msgmax;  // The largest a single message may be.
  int msgmnb;  // The most bytes one queue may hold.
  int msgmni;  // How many queues there may be at once.
  int msgssz;  // The size of a message segment.
  int msgtql;  // How many messages there may be in all.
  unsigned short msgseg;
};

#endif // LLVM_LIBC_TYPES_STRUCT_MSGINFO_H
