//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for ctanh
///
//===----------------------------------------------------------------------===//

#include "CHyperbolicTest.h"

#include "src/complex/ctanh.h"

LIST_CTANH_TESTS(_Complex double, double, LIBC_NAMESPACE::ctanh)
