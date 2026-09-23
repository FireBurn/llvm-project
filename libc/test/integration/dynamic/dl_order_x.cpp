//===-- The object which names the other two -----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Has nothing in it. It is linked against both of the others, A first, so
/// opening it loads A and then B, which needs A. See init_order_test.cpp.
///
//===----------------------------------------------------------------------===//

extern "C" void llvm_libc_order_x() {}
