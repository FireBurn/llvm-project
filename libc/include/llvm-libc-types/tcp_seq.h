//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of tcp_seq.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_TCP_SEQ_H
#define LLVM_LIBC_TYPES_TCP_SEQ_H

#include "../llvm-libc-macros/stdint-macros.h"

// A place in the stream of bytes a connection carries. It wraps.
typedef uint32_t tcp_seq;

#endif // LLVM_LIBC_TYPES_TCP_SEQ_H
