//===-- Macros defined in sys/ttydefaults.h header file -------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_MACROS_SYS_TTYDEFAULTS_MACROS_H
#define LLVM_LIBC_MACROS_SYS_TTYDEFAULTS_MACROS_H

// The settings a terminal is given when nothing else says what it should
// have. These are BSD's, and are the same everywhere they are defined, since
// what they describe is what a user expects a fresh terminal to do.

#define TTYDEF_IFLAG (BRKINT | ISTRIP | ICRNL | IMAXBEL | IXON | IXANY)
#define TTYDEF_OFLAG (OPOST | ONLCR | XTABS)
#define TTYDEF_LFLAG (ECHO | ICANON | ISIG | IEXTEN | ECHOE | ECHOKE | ECHOCTL)
#define TTYDEF_CFLAG (CREAD | CS7 | PARENB | HUPCL)
#define TTYDEF_SPEED (B9600)

// The control character a letter stands for, so that the defaults below can
// be written the way they are spoken.
#define CTRL(x) ((x) & 037)

#define CEOF CTRL('d')
#ifdef _POSIX_VDISABLE
#define CEOL _POSIX_VDISABLE
#else
#define CEOL '\0'
#endif
#define CERASE 0177
#define CINTR CTRL('c')
#ifdef _POSIX_VDISABLE
#define CSTATUS _POSIX_VDISABLE
#else
#define CSTATUS '\0'
#endif
#define CKILL CTRL('u')
#define CMIN 1
#define CQUIT                                                                  \
  034 // FS, ^\
#define CSUSP CTRL('z')
#define CTIME 0
#define CDSUSP CTRL('y')
#define CSTART CTRL('q')
#define CSTOP CTRL('s')
#define CLNEXT CTRL('v')
#define CDISCARD CTRL('o')
#define CWERASE CTRL('w')
#define CREPRINT CTRL('r')
#define CEOT CEOF
#define CBRK CEOL
#define CRPRNT CREPRINT
#define CFLUSH CDISCARD

#endif // LLVM_LIBC_MACROS_SYS_TTYDEFAULTS_MACROS_H
