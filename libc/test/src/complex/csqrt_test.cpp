//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains unittest for csqrt.
///
//===----------------------------------------------------------------------===//

#include "CSqrtTest.h"

#include "src/complex/csqrt.h"

LIST_CSQRT_TESTS(_Complex double, double, LIBC_NAMESPACE::csqrt)
