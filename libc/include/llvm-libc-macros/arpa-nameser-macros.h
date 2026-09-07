//===-- Macros defined in arpa/nameser.h header file ----------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_MACROS_ARPA_NAMESER_MACROS_H
#define LLVM_LIBC_MACROS_ARPA_NAMESER_MACROS_H

// The sizes a message and the names in it are held to.
#define NS_PACKETSZ 512    // What a message sent over UDP may reach.
#define NS_MAXDNAME 1025   // The longest name, written out.
#define NS_MAXMSG 65535    // The longest message.
#define NS_MAXCDNAME 255   // The longest name, as it is sent.
#define NS_MAXLABEL 63     // The longest one part of a name.
#define NS_MAXDHOSTNAME 256

// The fixed parts of a message.
#define NS_HFIXEDSZ 12  // The header.
#define NS_QFIXEDSZ 4   // What follows the name in a question.
#define NS_RRFIXEDSZ 10 // What follows the name in a record.

// The widths the message format is written in.
#define NS_INT32SZ 4
#define NS_INT16SZ 2
#define NS_INT8SZ 1
#define NS_INADDRSZ 4
#define NS_IN6ADDRSZ 16

// The two high bits of a length say the name goes on where it was written
// before, rather than here.
#define NS_CMPRSFLGS 0xc0

// Where a name server listens, over either transport.
#define NS_DEFAULTPORT 53

// The names these went by before the NS_ prefix, which programs still use.
#define PACKETSZ NS_PACKETSZ
#define MAXDNAME NS_MAXDNAME
#define MAXCDNAME NS_MAXCDNAME
#define MAXLABEL NS_MAXLABEL
#define HFIXEDSZ NS_HFIXEDSZ
#define QFIXEDSZ NS_QFIXEDSZ
#define RRFIXEDSZ NS_RRFIXEDSZ
#define INT32SZ NS_INT32SZ
#define INT16SZ NS_INT16SZ
#define INADDRSZ NS_INADDRSZ
#define IN6ADDRSZ NS_IN6ADDRSZ
#define INDIR_MASK NS_CMPRSFLGS
#define NAMESERVER_PORT NS_DEFAULTPORT

#endif // LLVM_LIBC_MACROS_ARPA_NAMESER_MACROS_H
