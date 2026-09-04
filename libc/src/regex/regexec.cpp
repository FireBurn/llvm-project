//===-- Implementation of regexec -----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/regex/regexec.h"

#include "hdr/regex_macros.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/regex/regex_engine.h"
#include "src/regex/regex_matcher.h"
#include "src/string/string_utils.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, regexec,
                   (const regex_t *__restrict preg,
                    const char *__restrict string, size_t nmatch,
                    regmatch_t *__restrict pmatch, int eflags)) {
  if (preg == nullptr || string == nullptr || preg->__internal == nullptr)
    return REG_NOMATCH;

  const auto *compiled = static_cast<const regex::Compiled *>(preg->__internal);
  const size_t length = internal::string_length(string);

  regex::Matcher matcher(*compiled, string, length, eflags);
  regex::Matcher::Span groups[regex::MAX_GROUPS];
  if (!matcher.search(0, groups))
    return REG_NOMATCH;

  // REG_NOSUB says the caller does not want offsets, so nothing is written
  // even if it passed a buffer.
  if (pmatch == nullptr || (compiled->cflags & REG_NOSUB) != 0)
    return 0;

  for (size_t i = 0; i < nmatch; ++i) {
    if (i < regex::MAX_GROUPS) {
      pmatch[i].rm_so = groups[i].start;
      pmatch[i].rm_eo = groups[i].end;
    } else {
      pmatch[i].rm_so = -1;
      pmatch[i].rm_eo = -1;
    }
  }
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL
