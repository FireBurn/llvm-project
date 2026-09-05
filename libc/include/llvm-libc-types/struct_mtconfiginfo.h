//===-- Definition of struct mtconfiginfo ---------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_MTCONFIGINFO_H
#define LLVM_LIBC_TYPES_STRUCT_MTCONFIGINFO_H

// The old configuration record of the tape driver, which the kernel's header
// no longer has but glibc's and musl's still do, as do the requests below
// that carry it.
struct mtconfiginfo {
  long mt_type;
  long ifc_type;
  unsigned short irqnr;
  unsigned short dmanr;
  unsigned short port;
  unsigned long debug;
  unsigned have_dens : 1;
  unsigned have_bsf : 1;
  unsigned have_fsr : 1;
  unsigned have_bsr : 1;
  unsigned have_eod : 1;
  unsigned have_seek : 1;
  unsigned have_tell : 1;
  unsigned have_ras1 : 1;
  unsigned have_ras2 : 1;
  unsigned have_ras3 : 1;
  unsigned have_qfa : 1;
  unsigned pad1 : 5;
  char reserved[10];
};

#endif // LLVM_LIBC_TYPES_STRUCT_MTCONFIGINFO_H
