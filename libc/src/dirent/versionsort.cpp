//===-- Implementation of versionsort -------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/dirent/versionsort.h"

#include "hdr/types/struct_dirent.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/string/strverscmp.h"

#include <dirent.h>

namespace LIBC_NAMESPACE_DECL {

// The ordering which reads a run of digits as the number it spells, so that
// "file10" comes after "file9".
LLVM_LIBC_FUNCTION(int, versionsort,
                   (const struct dirent **a, const struct dirent **b)) {
  return LIBC_NAMESPACE::strverscmp(&(*a)->d_name[0], &(*b)->d_name[0]);
}

} // namespace LIBC_NAMESPACE_DECL
