//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for ccos
///
//===----------------------------------------------------------------------===//

#include "CHyperbolicTest.h"

#include "src/complex/ccos.h"

LIST_CCOS_TESTS(_Complex double, double, LIBC_NAMESPACE::ccos)
