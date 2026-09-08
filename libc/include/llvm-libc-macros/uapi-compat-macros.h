//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Telling the kernel headers what this libc already defines.
///
//===----------------------------------------------------------------------===//

// No include guard: see the header this pulls in.

#ifdef __linux__
#include "linux/uapi-compat-macros.h"
#endif
