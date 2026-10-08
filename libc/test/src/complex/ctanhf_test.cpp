//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for ctanhf
///
//===----------------------------------------------------------------------===//

#include "CHyperbolicTest.h"

#include "src/complex/ctanhf.h"

LIST_CTANH_TESTS(_Complex float, float, LIBC_NAMESPACE::ctanhf)
