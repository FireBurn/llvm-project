//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Macros defined in netdb.h header file.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_MACROS_NETDB_MACROS_H
#define LLVM_LIBC_MACROS_NETDB_MACROS_H

#ifdef __linux__
#include "linux/netdb-macros.h"
#endif

// The last name lookup failure, which like errno is per thread and so is
// reached through a function.
#define h_errno (*__h_errno_location())

#endif // LLVM_LIBC_MACROS_NETDB_MACROS_H
