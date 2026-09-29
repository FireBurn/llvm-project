//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of ns_msg, a name server message being read.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_NS_MSG_H
#define LLVM_LIBC_TYPES_NS_MSG_H

#include "../llvm-libc-macros/stdint-macros.h"
#include "ns_sect.h"

/// What ns_initparse finds in a message, and how far ns_parserr has read.
/// The members are glibc's, which programs reach through the ns_msg_
/// macros.
typedef struct __ns_msg {
  const unsigned char *_msg; ///< The message.
  const unsigned char *_eom; ///< One past its end.
  uint16_t _id;
  uint16_t _flags;
  uint16_t _counts[4];               ///< Records in each section.
  const unsigned char *_sections[4]; ///< Where each section starts.
  ns_sect _sect;                     ///< The section being read.
  int _rrnum;                        ///< The record read next in it.
  const unsigned char *_msg_ptr;     ///< Where that record starts.
} ns_msg;

#endif // LLVM_LIBC_TYPES_NS_MSG_H
