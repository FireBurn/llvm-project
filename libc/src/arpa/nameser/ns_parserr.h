//===-- Implementation header for ns_parserr --------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_ARPA_NAMESER_NS_PARSERR_H
#define LLVM_LIBC_SRC_ARPA_NAMESER_NS_PARSERR_H

#include "hdr/types/ns_msg.h"
#include "hdr/types/ns_rr.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

int ns_parserr(ns_msg *handle, int section, int rrnum, ns_rr *rr);

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_ARPA_NAMESER_NS_PARSERR_H
