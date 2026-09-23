//===-- An object whose finaliser reports that it ran ---------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Writes a byte to a descriptor the program names when its fini array runs,
/// which for an object that is never closed is at exit.
///
//===----------------------------------------------------------------------===//

extern "C" {

// Defined by the program, which exports it.
extern int llvm_libc_exit_fd;

long write(int fd, const void *buffer, unsigned long size);

void llvm_libc_exit_destructor() { write(llvm_libc_exit_fd, "x", 1); }

// Placed in the array by hand, so that it is there whatever the compiler is
// configured to do with destructors.
__attribute__((section(".fini_array"), used)) void (*llvm_libc_exit_entry)() =
    llvm_libc_exit_destructor;
}
