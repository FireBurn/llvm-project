//===-- Definition of macros from sys/ioctl.h -----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_MACROS_LINUX_SYS_IOCTL_MACROS_H
#define LLVM_LIBC_MACROS_LINUX_SYS_IOCTL_MACROS_H

#include <asm/ioctls.h>
#include <linux/sockios.h>

// Which line discipline a terminal is put into. The kernel keeps these in
// <linux/tty.h>, which carries a great deal else besides, so they are stated
// here as the other libcs state them.
#define N_TTY 0
#define N_SLIP 1
#define N_MOUSE 2
#define N_PPP 3
#define N_STRIP 4
#define N_AX25 5
#define N_X25 6 // X.25 async
#define N_6PACK 7
#define N_MASC 8          // Mobitex module
#define N_R3964 9         // Simatic R3964 module
#define N_PROFIBUS_FDL 10 // Profibus
#define N_IRDA 11         // Linux IR
#define N_SMSBLOCK 12     // SMS block mode
#define N_HDLC 13         // synchronous HDLC
#define N_SYNC_PPP 14     // synchronous PPP
#define N_HCI 15          // Bluetooth HCI UART

#endif // LLVM_LIBC_MACROS_LINUX_SYS_IOCTL_MACROS_H
