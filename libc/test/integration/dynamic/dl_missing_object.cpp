//===-- An object to preload whose library is missing ---------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Needs a library which cannot be found, as a preload built for another C
/// library does. See preload_missing_test.cpp.
///
//===----------------------------------------------------------------------===//

extern "C" int llvm_libc_absent_value();

extern "C" int llvm_libc_missing_use() { return llvm_libc_absent_value(); }
