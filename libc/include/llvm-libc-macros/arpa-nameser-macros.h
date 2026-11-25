//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Macros defined in arpa/nameser.h header file.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_MACROS_ARPA_NAMESER_MACROS_H
#define LLVM_LIBC_MACROS_ARPA_NAMESER_MACROS_H

// DNS constants based on RFC 1035.
#define NS_PACKETSZ 512   // Maximum UDP message size (without EDNS).
#define NS_MAXDNAME 1025  // Buffer size for a domain name in text form.
#define NS_MAXDHOSTNAME 256 // Buffer size for a host name.
#define NS_MAXMSG 65535   // Maximum message size.
#define NS_MAXCDNAME 255  // Maximum domain name length in wire format.
#define NS_MAXLABEL 63    // Maximum label length.
#define NS_HFIXEDSZ 12    // Size of the message header.
#define NS_QFIXEDSZ 4     // Size of the fixed part of a question.
#define NS_RRFIXEDSZ 10   // Size of the fixed part of a resource record.
#define NS_INT32SZ 4      // Size of a 32-bit integer.
#define NS_INT16SZ 2      // Size of a 16-bit integer.
#define NS_INT8SZ 1       // Size of an 8-bit integer.
#define NS_INADDRSZ 4     // Size of an IPv4 address.
#define NS_IN6ADDRSZ 16   // Size of an IPv6 address.
#define NS_CMPRSFLGS 0xc0 // Flag bits marking a compressed name pointer.

// The EDNS option codes the resolver sends, and the flag in the options.
#define NS_OPT_DNSSEC_OK 0x8000U
#define NS_OPT_NSID 3
#define NS_OPT_PADDING 12

#define NS_DEFAULTPORT 53 // Server port for both UDP and TCP.

// Reading and writing the fields of a message. The sizes and the names are
// above; these are what a caller moves the bytes with.
#define NS_GET16(s, cp)                                                        \
  do {                                                                         \
    const unsigned char *t_cp = (const unsigned char *)(cp);                   \
    (s) = (__UINT16_TYPE__)(((unsigned)t_cp[0] << 8) | (unsigned)t_cp[1]);     \
    (cp) += NS_INT16SZ;                                                        \
  } while (0)

#define NS_GET32(l, cp)                                                        \
  do {                                                                         \
    const unsigned char *t_cp = (const unsigned char *)(cp);                   \
    (l) = (__UINT32_TYPE__)(((unsigned)t_cp[0] << 24) |                        \
                            ((unsigned)t_cp[1] << 16) |                        \
                            ((unsigned)t_cp[2] << 8) | (unsigned)t_cp[3]);     \
    (cp) += NS_INT32SZ;                                                        \
  } while (0)

#define NS_PUT16(s, cp)                                                        \
  do {                                                                         \
    __UINT16_TYPE__ t_s = (__UINT16_TYPE__)(s);                                \
    unsigned char *t_cp = (unsigned char *)(cp);                               \
    *t_cp++ = (unsigned char)(t_s >> 8);                                       \
    *t_cp = (unsigned char)t_s;                                                \
    (cp) += NS_INT16SZ;                                                        \
  } while (0)

#define NS_PUT32(l, cp)                                                        \
  do {                                                                         \
    __UINT32_TYPE__ t_l = (__UINT32_TYPE__)(l);                                \
    unsigned char *t_cp = (unsigned char *)(cp);                               \
    *t_cp++ = (unsigned char)(t_l >> 24);                                      \
    *t_cp++ = (unsigned char)(t_l >> 16);                                      \
    *t_cp++ = (unsigned char)(t_l >> 8);                                       \
    *t_cp = (unsigned char)t_l;                                                \
    (cp) += NS_INT32SZ;                                                        \
  } while (0)

// What ns_initparse and ns_parserr found.
#define ns_msg_id(handle) ((handle)._id + 0)
#define ns_msg_base(handle) ((handle)._msg + 0)
#define ns_msg_end(handle) ((handle)._eom + 0)
#define ns_msg_size(handle) ((handle)._eom - (handle)._msg)
#define ns_msg_count(handle, section) ((handle)._counts[section] + 0)
#define ns_rr_name(rr) (((rr).name[0] != '\0') ? (rr).name : ".")
#define ns_rr_type(rr) ((ns_type)((rr).type + 0))
#define ns_rr_class(rr) ((ns_class)((rr).rr_class + 0))
#define ns_rr_ttl(rr) ((rr).ttl + 0)
#define ns_rr_rdlen(rr) ((rr).rdlength + 0)
#define ns_rr_rdata(rr) ((rr).rdata + 0)

#define GETSHORT NS_GET16
#define GETLONG NS_GET32
#define PUTSHORT NS_PUT16
#define PUTLONG NS_PUT32

#endif // LLVM_LIBC_MACROS_ARPA_NAMESER_MACROS_H
