//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of ns_flag.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_NS_FLAG_H
#define LLVM_LIBC_TYPES_NS_FLAG_H

/// A header flag, as ns_msg_getflag takes it.
typedef enum __ns_flag {
  ns_f_qr,     ///< Question or response.
  ns_f_opcode, ///< Operation code.
  ns_f_aa,     ///< Authoritative answer.
  ns_f_tc,     ///< Truncated.
  ns_f_rd,     ///< Recursion desired.
  ns_f_ra,     ///< Recursion available.
  ns_f_z,      ///< Must be zero.
  ns_f_ad,     ///< Authentic data.
  ns_f_cd,     ///< Checking disabled.
  ns_f_rcode,  ///< Response code.
  ns_f_max,
} ns_flag;

#endif // LLVM_LIBC_TYPES_NS_FLAG_H
