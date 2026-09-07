//===-- Definition of macros from resolv.h --------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_MACROS_RESOLV_MACROS_H
#define LLVM_LIBC_MACROS_RESOLV_MACROS_H

// How much of the resolver's state there is room for.
#define MAXNS 3            // Servers read from the configuration.
#define MAXDFLSRCH 3       // Domains built from this machine's own name.
#define MAXDNSRCH 6        // Domains an unqualified name is tried in.
#define LOCALDOMAINPARTS 2 // Parts kept when building those from the name.
#define MAXRESOLVSORT 10   // Addresses the answers may be sorted by.

// The defaults, and the limits the configuration may not go past.
#define RES_TIMEOUT 5   // Seconds to wait for an answer.
#define RES_DFLRETRY 2  // Times to ask before giving up.
#define RES_MAXNDOTS 15 // As many as the field holding it can count.
#define RES_MAXRETRANS 30
#define RES_MAXRETRY 5
#define RES_MAXTIME 65535 // Milliseconds standing for no limit at all.

// The bits of the options field. Four of these name behaviour nothing
// implements any more; the values are kept so that a state written by
// something older still reads the same way.
#define RES_INIT 0x00000001        // The state has been filled in.
#define RES_DEBUG 0x00000002       // Say what is being asked and answered.
#define RES_AAONLY 0x00000004      // No longer does anything.
#define RES_USEVC 0x00000008       // Ask over TCP rather than UDP.
#define RES_PRIMARY 0x00000010     // No longer does anything.
#define RES_IGNTC 0x00000020       // Take a truncated answer as it stands.
#define RES_RECURSE 0x00000040     // Ask the server to do the work.
#define RES_DEFNAMES 0x00000080    // Add the local domain to a bare name.
#define RES_STAYOPEN 0x00000100    // Keep the TCP connection between queries.
#define RES_DNSRCH 0x00000200      // Try the name in each search domain.
#define RES_NOALIASES 0x00001000   // Ignore the HOSTALIASES file.
#define RES_ROTATE 0x00004000      // Start from a different server each time.
#define RES_NOCHECKNAME 0x00008000 // No longer does anything.
#define RES_KEEPTSIG 0x00010000    // No longer does anything.
#define RES_BLAST 0x00020000       // No longer does anything.
#define RES_USE_EDNS0 0x00100000   // Say how large an answer may be.
#define RES_SNGLKUP 0x00200000     // One question outstanding at a time.
#define RES_SNGLKUPREOP 0x00400000 // As above, with a new socket each time.
#define RES_USE_DNSSEC 0x00800000  // Ask for the signatures too.
#define RES_NOTLDQUERY 0x01000000  // Do not try a name with no dot in it.
#define RES_NORELOAD 0x02000000    // Do not read the configuration again.
#define RES_TRUSTAD 0x04000000     // Keep the authenticated data bit.
#define RES_NOAAAA 0x08000000      // Do not ask for IPv6 addresses.
#define RES_STRICTERR 0x10000000   // Report more of what went wrong.

#define RES_DEFAULT (RES_RECURSE | RES_DEFNAMES | RES_DNSRCH)

// Which of what passes to print, in the pfcode field.
#define RES_PRF_STATS 0x00000001
#define RES_PRF_UPDATE 0x00000002
#define RES_PRF_CLASS 0x00000004
#define RES_PRF_CMD 0x00000008
#define RES_PRF_QUES 0x00000010
#define RES_PRF_ANS 0x00000020
#define RES_PRF_AUTH 0x00000040
#define RES_PRF_ADD 0x00000080
#define RES_PRF_HEAD1 0x00000100
#define RES_PRF_HEAD2 0x00000200
#define RES_PRF_TTLID 0x00000400
#define RES_PRF_HEADX 0x00000800
#define RES_PRF_QUERY 0x00001000
#define RES_PRF_REPLY 0x00002000
#define RES_PRF_INIT 0x00004000

// The one state every program shares. It is per thread, so two threads
// looking things up at once do not tread on each other.
#define _res (*__res_state())

#endif // LLVM_LIBC_MACROS_RESOLV_MACROS_H
