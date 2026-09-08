//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains unittest for csqrtf.
///
//===----------------------------------------------------------------------===//

#include "CSqrtTest.h"

#include "src/complex/csqrtf.h"

LIST_CSQRT_TESTS(_Complex float, float, LIBC_NAMESPACE::csqrtf)
