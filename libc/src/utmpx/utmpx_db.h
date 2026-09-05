//===-- The utmpx database ------------------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_UTMPX_UTMPX_DB_H
#define LLVM_LIBC_SRC_UTMPX_UTMPX_DB_H

#include "hdr/types/struct_utmpx.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {
namespace utmpx_db {

// The file the calls read and write, which utmpxname may point elsewhere.
extern const char *path;

// The descriptor the iteration is on, or -1. The record last handed back
// lives here too, since the calls return a pointer into the library.
extern int fd;
extern struct utmpx entry;

// Opens the file if it is not already open, and rewinds it. Returns false
// if it could not be opened.
bool rewind();

// Closes it.
void close();

// Reads the next record. Returns false at the end of the file, or if
// nothing could be read.
bool read_next();

} // namespace utmpx_db
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_UTMPX_UTMPX_DB_H
