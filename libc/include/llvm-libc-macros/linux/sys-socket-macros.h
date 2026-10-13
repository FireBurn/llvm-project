//===-- Definition of macros from sys/socket.h ----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_MACROS_LINUX_SYS_SOCKET_MACROS_H
#define LLVM_LIBC_MACROS_LINUX_SYS_SOCKET_MACROS_H

// IEEE Std 1003.1-2017 - basedefs/sys_socket.h.html
// Macro values come from the Linux syscall interface.
//
// The AF_ names are defined in terms of the PF_ ones, as glibc does, because
// systemd scans for "#define AF_x AF_y" or "PF_y" to build its table of
// address families and finds none when AF_ names are numbers.

// 4.2BSD protocol families
#define PF_UNSPEC 0
#define PF_UNIX 1
#define PF_LOCAL 1
#define PF_INET 2
#define PF_INET6 10
#define PF_NETLINK 16
#define PF_BRIDGE 7
#define PF_PACKET 17
#define PF_VSOCK 40
#define PF_MPLS 28
#define PF_CAN 29
#define PF_ALG 38
#define PF_XDP 44
#define PF_AX25 3
#define PF_IPX 4
#define PF_APPLETALK 5
#define PF_NETROM 6
#define PF_ATMPVC 8
#define PF_X25 9
#define PF_ROSE 11
#define PF_DECnet 12
#define PF_NETBEUI 13
#define PF_SECURITY 14
#define PF_KEY 15
#define PF_ASH 18
#define PF_ECONET 19
#define PF_ATMSVC 20
#define PF_RDS 21
#define PF_SNA 22
#define PF_IRDA 23
#define PF_PPPOX 24
#define PF_WANPIPE 25
#define PF_LLC 26
#define PF_IB 27
#define PF_TIPC 30
#define PF_BLUETOOTH 31
#define PF_IUCV 32
#define PF_RXRPC 33
#define PF_ISDN 34
#define PF_PHONET 35
#define PF_IEEE802154 36
#define PF_CAIF 37
#define PF_NFC 39
#define PF_KCM 41
#define PF_QIPCRTR 42
#define PF_SMC 43
#define PF_MCTP 45
#define PF_MAX 46
#define PF_FILE PF_LOCAL
#define PF_ROUTE PF_NETLINK

#define AF_UNSPEC PF_UNSPEC   // Unspecified
#define AF_UNIX PF_UNIX       // Unix domain sockets
#define AF_LOCAL PF_LOCAL     // POSIX name for AF_UNIX
#define AF_INET PF_INET       // Internet IPv4 Protocol
#define AF_INET6 PF_INET6     // IP version 6
#define AF_NETLINK PF_NETLINK // Netlink sockets
#define AF_BRIDGE PF_BRIDGE   // Multiprotocol bridge
#define AF_PACKET PF_PACKET   // Packets straight off the device
#define AF_VSOCK PF_VSOCK     // Sockets between a guest and its host
#define AF_MPLS PF_MPLS       // Multiprotocol label switching
#define AF_CAN PF_CAN         // Controller area network
#define AF_ALG PF_ALG         // Kernel crypto
#define AF_XDP PF_XDP         // Express data path

// The rest of the families the kernel knows, at the numbers it gives them.
// AF_FILE and AF_ROUTE are the names 4.4BSD used for two of them.
#define AF_AX25 PF_AX25
#define AF_IPX PF_IPX
#define AF_APPLETALK PF_APPLETALK
#define AF_NETROM PF_NETROM
#define AF_ATMPVC PF_ATMPVC
#define AF_X25 PF_X25
#define AF_ROSE PF_ROSE
#define AF_DECnet PF_DECnet
#define AF_NETBEUI PF_NETBEUI
#define AF_SECURITY PF_SECURITY
#define AF_KEY PF_KEY
#define AF_ASH PF_ASH
#define AF_ECONET PF_ECONET
#define AF_ATMSVC PF_ATMSVC
#define AF_RDS PF_RDS
#define AF_SNA PF_SNA
#define AF_IRDA PF_IRDA
#define AF_PPPOX PF_PPPOX
#define AF_WANPIPE PF_WANPIPE
#define AF_LLC PF_LLC
#define AF_IB PF_IB
#define AF_TIPC PF_TIPC
#define AF_BLUETOOTH PF_BLUETOOTH
#define AF_IUCV PF_IUCV
#define AF_RXRPC PF_RXRPC
#define AF_ISDN PF_ISDN
#define AF_PHONET PF_PHONET
#define AF_IEEE802154 PF_IEEE802154
#define AF_CAIF PF_CAIF
#define AF_NFC PF_NFC
#define AF_KCM PF_KCM
#define AF_QIPCRTR PF_QIPCRTR
#define AF_SMC PF_SMC
#define AF_MCTP PF_MCTP
#define AF_MAX PF_MAX
#define AF_FILE PF_FILE
#define AF_ROUTE PF_ROUTE

#define SOCK_STREAM 1
#define SOCK_DGRAM 2
#define SOCK_RAW 3
#define SOCK_RDM 4
#define SOCK_SEQPACKET 5
#define SOCK_DCCP 6
#define SOCK_PACKET 10

#define SOCK_CLOEXEC 0x80000
#define SOCK_NONBLOCK 0x800

#define SOL_SOCKET 1

// How many connections a listening socket may have waiting by default.
#define SOMAXCONN 4096

// The other levels setsockopt takes, for options which belong to a
// protocol rather than to the socket itself. The ones which name a
// protocol carried over IP are that protocol's own number.
#define SOL_IP 0
#define SOL_TCP 6
#define SOL_UDP 17
#define SOL_IPV6 41
#define SOL_ICMPV6 58
#define SOL_SCTP 132
#define SOL_UDPLITE 136
#define SOL_RAW 255
#define SOL_DECNET 261
#define SOL_X25 262
#define SOL_PACKET 263
#define SOL_ATM 264
#define SOL_AAL 265
#define SOL_IRDA 266
#define SOL_NETBEUI 267
#define SOL_LLC 268
#define SOL_DCCP 269
#define SOL_NETLINK 270
#define SOL_TIPC 271
#define SOL_RXRPC 272
#define SOL_PPPOL2TP 273
#define SOL_BLUETOOTH 274
#define SOL_PNPIPE 275
#define SOL_RDS 276
#define SOL_IUCV 277
#define SOL_CAIF 278
#define SOL_ALG 279
#define SOL_NFC 280
#define SOL_KCM 281
#define SOL_TLS 282
#define SOL_XDP 283
#define SOL_MPTCP 284
#define SOL_MCTP 285
#define SOL_SMC 286
#define SOL_VSOCK 287

// The values of the following constants differ on some architectures
#if defined(__parisc__) || defined(__alpha__) || defined(__sparc__) ||         \
    defined(__powerpc__) || defined(__mips__)
#error "Please check SO_ constant values"
#endif

#define SO_DEBUG 1
#define SO_REUSEADDR 2
#define SO_TYPE 3
#define SO_ERROR 4
#define SO_DONTROUTE 5
#define SO_BROADCAST 6
#define SO_SNDBUF 7
#define SO_RCVBUF 8
#define SO_KEEPALIVE 9
#define SO_OOBINLINE 10
#define SO_NO_CHECK 11
#define SO_PRIORITY 12
#define SO_LINGER 13
#define SO_BSDCOMPAT 14
#define SO_REUSEPORT 15
#define SO_PASSCRED 16
#define SO_PEERCRED 17
#define SO_RCVLOWAT 18
#define SO_SNDLOWAT 19

// The "old" options use a 32-bit time_t on 32-bit systems.
#define SO_RCVTIMEO_OLD 20
#define SO_SNDTIMEO_OLD 21

#define SO_SECURITY_AUTHENTICATION 22
#define SO_SECURITY_ENCRYPTION_TRANSPORT 23
#define SO_SECURITY_ENCRYPTION_NETWORK 24
#define SO_BINDTODEVICE 25
#define SO_ATTACH_FILTER 26
#define SO_DETACH_FILTER 27
#define SO_GET_FILTER SO_ATTACH_FILTER
#define SO_PEERNAME 28
#define SO_TIMESTAMP_OLD 29
#define SO_ACCEPTCONN 30
#define SO_PEERSEC 31
#define SO_SNDBUFFORCE 32
#define SO_RCVBUFFORCE 33
#define SO_PASSSEC 34
#define SO_TIMESTAMPNS_OLD 35
#define SO_MARK 36
#define SO_TIMESTAMPING_OLD 37
#define SO_PROTOCOL 38
#define SO_DOMAIN 39
#define SO_RXQ_OVFL 40
#define SO_WIFI_STATUS 41
#define SCM_WIFI_STATUS SO_WIFI_STATUS
#define SO_PEEK_OFF 42
#define SO_NOFCS 43
#define SO_LOCK_FILTER 44
#define SO_SELECT_ERR_QUEUE 45
#define SO_BUSY_POLL 46
#define SO_MAX_PACING_RATE 47
#define SO_BPF_EXTENSIONS 48
#define SO_INCOMING_CPU 49
#define SO_ATTACH_BPF 50
#define SO_DETACH_BPF SO_DETACH_FILTER
#define SO_ATTACH_REUSEPORT_CBPF 51
#define SO_ATTACH_REUSEPORT_EBPF 52
#define SO_CNX_ADVICE 53
#define SCM_TIMESTAMPING_OPT_STATS 54
#define SO_MEMINFO 55
#define SO_INCOMING_NAPI_ID 56
#define SO_COOKIE 57
#define SCM_TIMESTAMPING_PKTINFO 58
#define SO_PEERGROUPS 59
#define SO_ZEROCOPY 60
#define SO_TXTIME 61
#define SCM_TXTIME SO_TXTIME
#define SO_BINDTOIFINDEX 62

// These are the "new" options, which assume a 64-bit time_t, regardless of the
// pointer size.
#define SO_TIMESTAMP_NEW 63
#define SO_TIMESTAMPNS_NEW 64
#define SO_TIMESTAMPING_NEW 65
#define SO_RCVTIMEO_NEW 66
#define SO_SNDTIMEO_NEW 67

// Where time_t is already 64 bits the old options are the same thing, and
// they are the ones glibc gives the plain names.
#if defined(__LP64__)
#define SO_TIMESTAMP SO_TIMESTAMP_OLD
#define SO_TIMESTAMPNS SO_TIMESTAMPNS_OLD
#define SO_TIMESTAMPING SO_TIMESTAMPING_OLD
#define SO_RCVTIMEO SO_RCVTIMEO_OLD
#define SO_SNDTIMEO SO_SNDTIMEO_OLD
#else
#define SO_TIMESTAMP SO_TIMESTAMP_NEW
#define SO_TIMESTAMPNS SO_TIMESTAMPNS_NEW
#define SO_TIMESTAMPING SO_TIMESTAMPING_NEW
#define SO_RCVTIMEO SO_RCVTIMEO_NEW
#define SO_SNDTIMEO SO_SNDTIMEO_NEW
#endif

// The control message a timestamp arrives in is named by the option which
// asked for it.
#define SCM_TIMESTAMP SO_TIMESTAMP
#define SCM_TIMESTAMPNS SO_TIMESTAMPNS
#define SCM_TIMESTAMPING SO_TIMESTAMPING

#define SO_DETACH_REUSEPORT_BPF 68
#define SO_PREFER_BUSY_POLL 69
#define SO_BUSY_POLL_BUDGET 70
#define SO_NETNS_COOKIE 71
#define SO_BUF_LOCK 72
#define SO_RESERVE_MEM 73
#define SO_TXREHASH 74
#define SO_RCVMARK 75
#define SO_PASSPIDFD 76
#define SO_PEERPIDFD 77
#define SO_DEVMEM_LINEAR 78
#define SO_DEVMEM_DMABUF 79
#define SO_DEVMEM_DONTNEED 80
#define SO_RCVPRIORITY 82
#define SO_PASSRIGHTS 83
#define SO_INQ 84

#define SHUT_RD 0
#define SHUT_WR 1
#define SHUT_RDWR 2

#define SCM_RIGHTS 1
#define SCM_TIMESTAMP SO_TIMESTAMP
#define SCM_TIMESTAMPNS SO_TIMESTAMPNS
#define SCM_TIMESTAMPING SO_TIMESTAMPING
#define SCM_CREDENTIALS 2
#define SCM_SECURITY 3
#define SCM_PIDFD 4

#define MSG_OOB 0x01
#define MSG_PEEK 0x02
#define MSG_DONTROUTE 0x04
#define MSG_CTRUNC 0x08
#define MSG_PROXY 0x10
#define MSG_TRUNC 0x20
#define MSG_DONTWAIT 0x40
#define MSG_EOR 0x80
#define MSG_WAITALL 0x100
#define MSG_FIN 0x200
#define MSG_SYN 0x400
#define MSG_CONFIRM 0x800
#define MSG_RST 0x1000
#define MSG_ERRQUEUE 0x2000
#define MSG_NOSIGNAL 0x4000
#define MSG_MORE 0x8000
#define MSG_WAITFORONE 0x10000
#define MSG_BATCH 0x40000
#define MSG_SOCK_DEVMEM 0x2000000
#define MSG_ZEROCOPY 0x4000000
#define MSG_FASTOPEN 0x20000000
#define MSG_CMSG_CLOEXEC 0x40000000

#define CMSG_ALIGN(len) (((len) + sizeof(size_t) - 1) & ~(sizeof(size_t) - 1))
#define CMSG_LEN(len) (sizeof(struct cmsghdr) + (len))
#define CMSG_SPACE(len) (sizeof(struct cmsghdr) + CMSG_ALIGN(len))

#define CMSG_FIRSTHDR(msg)                                                     \
  ((msg)->msg_controllen >= sizeof(struct cmsghdr)                             \
       ? (struct cmsghdr *)(msg)->msg_control                                  \
       : 0)
#define __CMSG_NXTHDR_CANDIDATE(cmsg)                                          \
  ((struct cmsghdr *)((unsigned char *)(cmsg) + CMSG_ALIGN((cmsg)->cmsg_len)))
#define CMSG_NXTHDR(msg, cmsg)                                                 \
  ((char *)(__CMSG_NXTHDR_CANDIDATE(cmsg) + 1) <=                              \
           ((char *)((msg)->msg_control) + (msg)->msg_controllen)              \
       ? __CMSG_NXTHDR_CANDIDATE(cmsg)                                         \
       : 0)
#define CMSG_DATA(cmsg) ((unsigned char *)((struct cmsghdr *)(cmsg) + 1))

#endif // LLVM_LIBC_MACROS_LINUX_SYS_SOCKET_MACROS_H
