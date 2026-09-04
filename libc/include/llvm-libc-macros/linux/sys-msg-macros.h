//===-- Macros defined in sys/msg.h header file ---------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_MACROS_LINUX_SYS_MSG_MACROS_H
#define LLVM_LIBC_MACROS_LINUX_SYS_MSG_MACROS_H

// The flags msgrcv takes, over and above the IPC_ ones.
#define MSG_NOERROR 010000 // Truncate a message too large rather than fail.
#define MSG_EXCEPT 020000  // Take any message but the type named.
#define MSG_COPY 040000    // Read a message without taking it off the queue.

// The msgctl commands which are Linux's own.
#define MSG_STAT 11
#define MSG_INFO 12
#define MSG_STAT_ANY 13

// The member which says how many bytes are on the queue is private, since
// nothing standard names it, but code which does reaches for it under this
// name.
#define msg_cbytes __msg_cbytes

#endif // LLVM_LIBC_MACROS_LINUX_SYS_MSG_MACROS_H
