//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_FTW_H
#define LLVM_LIBC_TYPES_STRUCT_FTW_H

/// Where in the walk the callback was called from.
struct FTW {
  /// Offset within the path of the last component, so the callback can name
  /// the entry without searching the path itself.
  int base;
  /// How far below the directory the walk started from this entry is, which
  /// is zero for that directory.
  int level;
};

#endif // LLVM_LIBC_TYPES_STRUCT_FTW_H
