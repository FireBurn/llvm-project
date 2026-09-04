//===-- Implementation of alphasort ---------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/dirent/alphasort.h"

#include "hdr/types/struct_dirent.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"

#include <dirent.h>

namespace LIBC_NAMESPACE_DECL {

// The ordering scandir uses unless a caller asks for another: the names
// compared byte by byte.
LLVM_LIBC_FUNCTION(int, alphasort,
                   (const struct dirent **a, const struct dirent **b)) {
  const char *left = &(*a)->d_name[0];
  const char *right = &(*b)->d_name[0];
  for (; *left != '\0' && *left == *right; ++left, ++right)
    ;
  return static_cast<int>(static_cast<unsigned char>(*left)) -
         static_cast<int>(static_cast<unsigned char>(*right));
}

} // namespace LIBC_NAMESPACE_DECL
