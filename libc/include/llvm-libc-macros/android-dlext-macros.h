//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Macros defined in android/dlext.h header file.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_MACROS_ANDROID_DLEXT_MACROS_H
#define LLVM_LIBC_MACROS_ANDROID_DLEXT_MACROS_H

// Flags of android_dlextinfo::flags.
#define ANDROID_DLEXT_RESERVED_ADDRESS 0x1
#define ANDROID_DLEXT_RESERVED_ADDRESS_HINT 0x2
#define ANDROID_DLEXT_WRITE_RELRO 0x4
#define ANDROID_DLEXT_USE_RELRO 0x8
#define ANDROID_DLEXT_USE_LIBRARY_FD 0x10
#define ANDROID_DLEXT_USE_LIBRARY_FD_OFFSET 0x20
#define ANDROID_DLEXT_FORCE_LOAD 0x40
#define ANDROID_DLEXT_USE_NAMESPACE 0x200
#define ANDROID_DLEXT_RESERVED_ADDRESS_RECURSIVE 0x400
#define ANDROID_DLEXT_VALID_FLAG_BITS                                          \
  (ANDROID_DLEXT_RESERVED_ADDRESS | ANDROID_DLEXT_RESERVED_ADDRESS_HINT |      \
   ANDROID_DLEXT_WRITE_RELRO | ANDROID_DLEXT_USE_RELRO |                       \
   ANDROID_DLEXT_USE_LIBRARY_FD | ANDROID_DLEXT_USE_LIBRARY_FD_OFFSET |        \
   ANDROID_DLEXT_FORCE_LOAD | ANDROID_DLEXT_USE_NAMESPACE |                    \
   ANDROID_DLEXT_RESERVED_ADDRESS_RECURSIVE)

#define ANDROID_NAMESPACE_TYPE_REGULAR 0
#define ANDROID_NAMESPACE_TYPE_ISOLATED 1
#define ANDROID_NAMESPACE_TYPE_SHARED 2
#define ANDROID_NAMESPACE_TYPE_SHARED_ISOLATED 3
#define ANDROID_NAMESPACE_TYPE_EXEMPT_LIST_ENABLED 0x08000000
#define ANDROID_NAMESPACE_TYPE_ALSO_USED_AS_ANONYMOUS 0x10000000

#endif // LLVM_LIBC_MACROS_ANDROID_DLEXT_MACROS_H
