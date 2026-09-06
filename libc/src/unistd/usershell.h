//===-- The list of login shells --------------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_UNISTD_USERSHELL_H
#define LLVM_LIBC_SRC_UNISTD_USERSHELL_H

#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {
namespace usershell {

// The longest line the file may hold. A shell named by more than this is one
// nothing can run anyway.
constexpr unsigned LINE_SIZE = 1024;

// Which file the list is read from, which is /etc/shells unless a test says
// otherwise.
void TESTONLY_set_shells_path(const char *path);
const char *path();

// Starts the walk again from the top of the file.
void rewind();
// Closes it.
void close();
// The next shell, or null at the end of the list. The string is the library's
// and is overwritten by the following call.
const char *next();

} // namespace usershell
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_UNISTD_USERSHELL_H
