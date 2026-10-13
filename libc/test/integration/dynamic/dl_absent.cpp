//===-- A library the preloaded object needs and nothing can find ----------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Built into a directory nobody searches, so that the object which needs it
/// has a library that is missing. See preload_missing_test.cpp.
///
//===----------------------------------------------------------------------===//

extern "C" int llvm_libc_absent_value() { return 1; }
