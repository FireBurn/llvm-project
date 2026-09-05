//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of struct spwd.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_SPWD_H
#define LLVM_LIBC_TYPES_STRUCT_SPWD_H

// One entry of the shadow password database. The counts of days are from
// the start of 1970, and a field the file left empty reads as -1.
struct spwd {
  char *sp_namp;         // The login name.
  char *sp_pwdp;         // The hashed password.
  long sp_lstchg;        // When it was last changed.
  long sp_min;           // Days before it may be changed again.
  long sp_max;           // Days before it has to be.
  long sp_warn;          // Days of warning before that.
  long sp_inact;         // Days the account stays usable once expired.
  long sp_expire;        // When the account itself expires.
  unsigned long sp_flag; // Reserved.
};

#endif // LLVM_LIBC_TYPES_STRUCT_SPWD_H
