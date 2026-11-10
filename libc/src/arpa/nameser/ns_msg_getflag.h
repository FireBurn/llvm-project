//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation header for ns_msg_getflag
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_ARPA_NAMESER_NS_MSG_GETFLAG_H
#define LLVM_LIBC_SRC_ARPA_NAMESER_NS_MSG_GETFLAG_H

#include "hdr/types/ns_msg.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

int ns_msg_getflag(ns_msg handle, int flag);

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_ARPA_NAMESER_NS_MSG_GETFLAG_H
