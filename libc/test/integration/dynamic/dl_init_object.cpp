//===-- Shared object with DT_INIT and DT_FINI for the dynamic tests ------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Records the order in which its initialisers and finalisers run. DT_INIT
/// is given to the linker by name, and the constructor and destructor are put
/// in the init and fini arrays.
///
//===----------------------------------------------------------------------===//

extern "C" {

int llvm_libc_dl_init_count;
int llvm_libc_dl_init_order[4];

// Defined by the program, which exports them, since this object is gone by
// the time its finalisers have run.
extern int llvm_libc_host_fini_count;
extern int llvm_libc_host_fini_order[4];

void llvm_libc_dl_init() {
  llvm_libc_dl_init_order[llvm_libc_dl_init_count++] = 1;
}

void llvm_libc_dl_constructor() {
  llvm_libc_dl_init_order[llvm_libc_dl_init_count++] = 2;
}

void llvm_libc_dl_destructor() {
  llvm_libc_host_fini_order[llvm_libc_host_fini_count++] = 3;
}

// Placed in the arrays by hand, so that they are there whatever the compiler
// is configured to do with constructors.
__attribute__((section(".init_array"), used)) void (*llvm_libc_dl_init_entry)() =
    llvm_libc_dl_constructor;
__attribute__((section(".fini_array"), used)) void (*llvm_libc_dl_fini_entry)() =
    llvm_libc_dl_destructor;

void llvm_libc_dl_fini() {
  llvm_libc_host_fini_order[llvm_libc_host_fini_count++] = 4;
}
}
