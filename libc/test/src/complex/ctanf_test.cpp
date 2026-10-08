//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for ctanf
///
//===----------------------------------------------------------------------===//

#include "CHyperbolicTest.h"

#include "src/complex/ctanf.h"

LIST_CTAN_TESTS(_Complex float, float, LIBC_NAMESPACE::ctanf)
