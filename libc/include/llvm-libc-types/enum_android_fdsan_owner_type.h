//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of enum android_fdsan_owner_type.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_ENUM_ANDROID_FDSAN_OWNER_TYPE_H
#define LLVM_LIBC_TYPES_ENUM_ANDROID_FDSAN_OWNER_TYPE_H

enum android_fdsan_owner_type {
  ANDROID_FDSAN_OWNER_TYPE_GENERIC_00 = 0,
  ANDROID_FDSAN_OWNER_TYPE_GENERIC_FF = 255,
  ANDROID_FDSAN_OWNER_TYPE_FILE = 1,
  ANDROID_FDSAN_OWNER_TYPE_DIR = 2,
  ANDROID_FDSAN_OWNER_TYPE_UNIQUE_FD = 3,
  ANDROID_FDSAN_OWNER_TYPE_SQLITE = 4,
  ANDROID_FDSAN_OWNER_TYPE_FILEINPUTSTREAM = 5,
  ANDROID_FDSAN_OWNER_TYPE_FILEOUTPUTSTREAM = 6,
  ANDROID_FDSAN_OWNER_TYPE_RANDOMACCESSFILE = 7,
  ANDROID_FDSAN_OWNER_TYPE_PARCELFILEDESCRIPTOR = 8,
  ANDROID_FDSAN_OWNER_TYPE_ART_FDFILE = 9,
  ANDROID_FDSAN_OWNER_TYPE_DATAGRAMSOCKETIMPL = 10,
  ANDROID_FDSAN_OWNER_TYPE_SOCKETIMPL = 11,
  ANDROID_FDSAN_OWNER_TYPE_ZIPARCHIVE = 12,
  ANDROID_FDSAN_OWNER_TYPE_NATIVE_HANDLE = 13,
  ANDROID_FDSAN_OWNER_TYPE_PARCEL = 14,
};

#endif // LLVM_LIBC_TYPES_ENUM_ANDROID_FDSAN_OWNER_TYPE_H
