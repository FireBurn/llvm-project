//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of ns_sect.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_NS_SECT_H
#define LLVM_LIBC_TYPES_NS_SECT_H

/// A section of a name server message, one of the ns_s_ values. The values are
/// an unnamed enumeration, so this is the type they convert to.
typedef int ns_sect;

#endif // LLVM_LIBC_TYPES_NS_SECT_H
