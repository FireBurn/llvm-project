//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of HEADER, the fixed part of a name server message.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_HEADER_H
#define LLVM_LIBC_TYPES_HEADER_H

/// The twelve bytes every message from a name server starts with. The
/// members are laid out in the order the bytes carry them, which is why the
/// two bytes of flags are written differently on a machine of each order.
typedef struct {
  /// What the message is a reply to.
  unsigned id : 16;
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  unsigned qr : 1;     ///< Set on a reply.
  unsigned opcode : 4; ///< What was asked for.
  unsigned aa : 1;     ///< The answer comes from a server for the name.
  unsigned tc : 1;     ///< The reply did not fit and was cut short.
  unsigned rd : 1;     ///< The asker wants the server to ask further.
  unsigned ra : 1;     ///< The server is willing to.
  unsigned unused : 1;
  unsigned ad : 1;    ///< The answer was checked and holds.
  unsigned cd : 1;    ///< The asker does not want it checked.
  unsigned rcode : 4; ///< How the request turned out.
#else
  unsigned rd : 1;
  unsigned tc : 1;
  unsigned aa : 1;
  unsigned opcode : 4;
  unsigned qr : 1;
  unsigned rcode : 4;
  unsigned cd : 1;
  unsigned ad : 1;
  unsigned unused : 1;
  unsigned ra : 1;
#endif
  unsigned qdcount : 16; ///< How many questions follow.
  unsigned ancount : 16; ///< How many answers.
  unsigned nscount : 16; ///< How many records naming servers.
  unsigned arcount : 16; ///< How many records added alongside.
} HEADER;

#endif // LLVM_LIBC_TYPES_HEADER_H
