//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/stdlib/random_util.h"

namespace LIBC_NAMESPACE_DECL {
namespace random_internal {

// The table every caller shares until initstate says otherwise, with the
// word in front which says what it is. The 128 byte size is what a program
// which never calls initstate gets.
int32_t default_table[KINDS[3].degree + 1];

State state = {default_table + 1,   3, KINDS[3].degree, KINDS[3].separation,
               KINDS[3].separation, 0};

bool seeded = false;

} // namespace random_internal
} // namespace LIBC_NAMESPACE_DECL
