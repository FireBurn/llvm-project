//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of the macros BSD and bionic add to sys/endian.h.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_MACROS_SYS_ENDIAN_MACROS_H
#define LLVM_LIBC_MACROS_SYS_ENDIAN_MACROS_H

#include "endian-macros.h"

#define betoh16(x) be16toh(x)
#define betoh32(x) be32toh(x)
#define betoh64(x) be64toh(x)
#define letoh16(x) le16toh(x)
#define letoh32(x) le32toh(x)
#define letoh64(x) le64toh(x)

#define htonq(x) htobe64(x)
#define ntohq(x) be64toh(x)

#define swap16(x) __builtin_bswap16((x))
#define swap32(x) __builtin_bswap32((x))
#define swap64(x) __builtin_bswap64((x))

#endif // LLVM_LIBC_MACROS_SYS_ENDIAN_MACROS_H
