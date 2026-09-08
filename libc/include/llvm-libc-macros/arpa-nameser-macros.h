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
#define NS_PACKETSZ 512  // What a message sent over UDP may reach.
#define NS_MAXDNAME 1025 // The longest name, written out.
#define NS_MAXMSG 65535  // The longest message.
#define NS_MAXCDNAME 255 // The longest name, as it is sent.
#define NS_MAXLABEL 63   // The longest one part of a name.
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

// The names the record types went by before the ns_t_ prefix, which programs
// still use. Each is the enumerator of the same name.
#define T_A ns_t_a
#define T_NS ns_t_ns
#define T_CNAME ns_t_cname
#define T_SOA ns_t_soa
#define T_PTR ns_t_ptr
#define T_MX ns_t_mx
#define T_TXT ns_t_txt
#define T_AAAA ns_t_aaaa
#define T_SRV ns_t_srv
#define T_OPT ns_t_opt
#define T_MD ns_t_md
#define T_MF ns_t_mf
#define T_MB ns_t_mb
#define T_MG ns_t_mg
#define T_MR ns_t_mr
#define T_NULL ns_t_null
#define T_WKS ns_t_wks
#define T_HINFO ns_t_hinfo
#define T_MINFO ns_t_minfo
#define T_RP ns_t_rp
#define T_AFSDB ns_t_afsdb
#define T_X25 ns_t_x25
#define T_ISDN ns_t_isdn
#define T_RT ns_t_rt
#define T_NSAP ns_t_nsap
#define T_NSAP_PTR ns_t_nsap_ptr
#define T_SIG ns_t_sig
#define T_KEY ns_t_key
#define T_PX ns_t_px
#define T_GPOS ns_t_gpos
#define T_LOC ns_t_loc
#define T_NXT ns_t_nxt
#define T_EID ns_t_eid
#define T_NIMLOC ns_t_nimloc
#define T_ATMA ns_t_atma
#define T_NAPTR ns_t_naptr
#define T_KX ns_t_kx
#define T_CERT ns_t_cert
#define T_A6 ns_t_a6
#define T_DNAME ns_t_dname
#define T_SINK ns_t_sink
#define T_APL ns_t_apl
#define T_DS ns_t_ds
#define T_SSHFP ns_t_sshfp
#define T_IPSECKEY ns_t_ipseckey
#define T_RRSIG ns_t_rrsig
#define T_NSEC ns_t_nsec
#define T_DNSKEY ns_t_dnskey
#define T_DHCID ns_t_dhcid
#define T_NSEC3 ns_t_nsec3
#define T_NSEC3PARAM ns_t_nsec3param
#define T_TLSA ns_t_tlsa
#define T_SMIMEA ns_t_smimea
#define T_HIP ns_t_hip
#define T_NINFO ns_t_ninfo
#define T_RKEY ns_t_rkey
#define T_TALINK ns_t_talink
#define T_CDS ns_t_cds
#define T_CDNSKEY ns_t_cdnskey
#define T_OPENPGPKEY ns_t_openpgpkey
#define T_CSYNC ns_t_csync
#define T_SPF ns_t_spf
#define T_UINFO ns_t_uinfo
#define T_UID ns_t_uid
#define T_GID ns_t_gid
#define T_UNSPEC ns_t_unspec
#define T_NID ns_t_nid
#define T_L32 ns_t_l32
#define T_L64 ns_t_l64
#define T_LP ns_t_lp
#define T_EUI48 ns_t_eui48
#define T_EUI64 ns_t_eui64
#define T_TKEY ns_t_tkey
#define T_TSIG ns_t_tsig
#define T_IXFR ns_t_ixfr
#define T_AXFR ns_t_axfr
#define T_MAILB ns_t_mailb
#define T_MAILA ns_t_maila
#define T_URI ns_t_uri
#define T_CAA ns_t_caa
#define T_AVC ns_t_avc
#define T_TA ns_t_ta
#define T_DLV ns_t_dlv
#define T_ANY ns_t_any

// The classes, likewise.
#define C_IN ns_c_in
#define C_CHAOS ns_c_chaos
#define C_HS ns_c_hs
#define C_NONE ns_c_none
#define C_ANY ns_c_any

// The sections of an update message, under the names they had before the
// prefix. The four are the same numbers as the sections of a query.
#define S_ZONE ns_s_zn
#define S_PREREQ ns_s_pr
#define S_UPDATE ns_s_ud
#define S_ADDT ns_s_ar

// The opcodes and the response codes, likewise.
#define QUERY ns_o_query
#define IQUERY ns_o_iquery
#define STATUS ns_o_status
#define NS_NOTIFY_OP ns_o_notify
#define NS_UPDATE_OP ns_o_update

#define NOERROR ns_r_noerror
#define FORMERR ns_r_formerr
#define SERVFAIL ns_r_servfail
#define NXDOMAIN ns_r_nxdomain
#define NOTIMP ns_r_notimpl
#define REFUSED ns_r_refused

#endif // LLVM_LIBC_MACROS_ARPA_NAMESER_MACROS_H
