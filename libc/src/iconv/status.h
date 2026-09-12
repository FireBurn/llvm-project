//===-- What a step of an iconv conversion did ------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_ICONV_STATUS_H
#define LLVM_LIBC_SRC_ICONV_STATUS_H

#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {
namespace iconv_internal {

// What a step of a conversion ended up doing.
enum class Status {
  OK,
  NONE,       // The bytes were a byte order mark rather than a character.
  INCOMPLETE, // The input ran out part way through a character.
  INVALID,    // The bytes are not a character in this set.
  SEQUENCE,   // The bytes stand for several characters. |out| is their code.
  FULL,       // There is no room in the output.
};

} // namespace iconv_internal
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_ICONV_STATUS_H
