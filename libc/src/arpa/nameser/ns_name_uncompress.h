//===-- Implementation header for ns_name_uncompress ------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_ARPA_NAMESER_NS_NAME_UNCOMPRESS_H
#define LLVM_LIBC_SRC_ARPA_NAMESER_NS_NAME_UNCOMPRESS_H

#include "hdr/types/size_t.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

int ns_name_uncompress(const unsigned char *msg, const unsigned char *eom,
                       const unsigned char *src, char *dst, size_t dstsiz);

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_ARPA_NAMESER_NS_NAME_UNCOMPRESS_H
