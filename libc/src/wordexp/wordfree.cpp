//===-- Implementation of wordfree ----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/wordexp/wordfree.h"
#include "hdr/func/free.h"
#include "hdr/types/wordexp_t.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(void, wordfree, (wordexp_t * we)) {
  if (we == nullptr || we->we_wordv == nullptr)
    return;

  // The empty slots at the front were never filled in, so start past them.
  char **word = we->we_wordv + we->we_offs;
  for (; *word != nullptr; ++word)
    free(*word);

  free(we->we_wordv);
  we->we_wordv = nullptr;
  we->we_wordc = 0;
}

} // namespace LIBC_NAMESPACE_DECL
