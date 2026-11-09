//===-- Shared object that needs the namespace test object ----------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Needs libnstest.so by name, so which copy of that it gets is up to the
/// namespace it is loaded in.
///
//===----------------------------------------------------------------------===//

extern "C" {

int llvm_libc_ns_which();

int llvm_libc_ns_user() { return 100 + llvm_libc_ns_which(); }
}
