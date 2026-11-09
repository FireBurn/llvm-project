//===-- Shared object that says which copy of itself it is ----------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Built twice, in two directories under the one name, so that which of them
/// a namespace loaded can be told from what it answers.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TEST_NS_WHICH
#error "LLVM_LIBC_TEST_NS_WHICH is the number this copy answers with"
#endif

extern "C" int llvm_libc_ns_which() { return LLVM_LIBC_TEST_NS_WHICH; }
