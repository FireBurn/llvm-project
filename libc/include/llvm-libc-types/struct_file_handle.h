//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of struct file_handle.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_FILE_HANDLE_H
#define LLVM_LIBC_TYPES_STRUCT_FILE_HANDLE_H

// A name for a file which does not depend on where it is in the tree.
// name_to_handle_at fills one in and open_by_handle_at opens it again. The
// caller sets handle_bytes to the room it has; the kernel sets it to what
// the handle took, and says EOVERFLOW where that was not enough.
struct file_handle {
  unsigned int handle_bytes;
  int handle_type;
  unsigned char f_handle[];
};

#endif // LLVM_LIBC_TYPES_STRUCT_FILE_HANDLE_H
