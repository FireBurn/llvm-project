//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation header for ns_sprintrr
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_ARPA_NAMESER_NS_SPRINTRR_H
#define LLVM_LIBC_SRC_ARPA_NAMESER_NS_SPRINTRR_H

#include "hdr/types/ns_msg.h"
#include "hdr/types/ns_rr.h"
#include "hdr/types/size_t.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

int ns_sprintrr(const ns_msg *handle, const ns_rr *rr, const char *name_ctx,
                const char *origin, char *buf, size_t buflen);

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_ARPA_NAMESER_NS_SPRINTRR_H
