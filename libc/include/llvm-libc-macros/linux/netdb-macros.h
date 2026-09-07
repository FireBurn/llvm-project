//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Macros defined in netdb.h header file for Linux.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_MACROS_LINUX_NETDB_MACROS_H
#define LLVM_LIBC_MACROS_LINUX_NETDB_MACROS_H

#define EAI_BADFLAGS -1
#define EAI_NONAME -2
#define EAI_AGAIN -3
#define EAI_FAIL -4
#define EAI_FAMILY -6
#define EAI_SOCKTYPE -7
#define EAI_SERVICE -8
#define EAI_MEMORY -10
#define EAI_SYSTEM -11
#define EAI_OVERFLOW -12

// Two the GNU library added, which code written against it uses.
#define EAI_NODATA -5
#define EAI_ADDRFAMILY -9

// Flags for the ai_flags field of struct addrinfo. The values are the ones
// Linux uses, which is what anything already built expects to see.
#define AI_PASSIVE 0x0001
#define AI_CANONNAME 0x0002
#define AI_NUMERICHOST 0x0004
#define AI_V4MAPPED 0x0008
#define AI_ALL 0x0010
#define AI_ADDRCONFIG 0x0020
#define AI_NUMERICSERV 0x0400

// Flags for getnameinfo.
#define NI_NUMERICHOST 1
#define NI_NUMERICSERV 2
#define NI_NOFQDN 4
#define NI_NAMEREQD 8
#define NI_DGRAM 16

// How much room getnameinfo may need for what it writes.
#define NI_MAXHOST 1025
#define NI_MAXSERV 32

// The values gethostbyname reports failures with, through h_errno.
#define HOST_NOT_FOUND 1
#define TRY_AGAIN 2
#define NO_RECOVERY 3
#define NO_DATA 4
#define NO_ADDRESS NO_DATA

#endif // LLVM_LIBC_MACROS_LINUX_NETDB_MACROS_H
