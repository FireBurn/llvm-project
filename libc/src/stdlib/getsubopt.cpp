//===-- Implementation of getsubopt ---------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/stdlib/getsubopt.h"

#include "hdr/types/size_t.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/string/string_utils.h"

namespace LIBC_NAMESPACE_DECL {

// Reads one suboption from a comma separated list, of the kind mount options
// are written in: "rw,size=10,noexec". The string is taken apart where it
// stands, so the commas and the equals sign become terminators and the
// pointers handed back name pieces of the caller's own buffer.
LLVM_LIBC_FUNCTION(int, getsubopt,
                   (char **optionp, char *const *tokens, char **valuep)) {
  *valuep = nullptr;

  if (optionp == nullptr || *optionp == nullptr || **optionp == '\0')
    return -1;

  char *start = *optionp;
  char *comma = internal::strchr_implementation(start, ',');
  if (comma != nullptr) {
    *comma = '\0';
    *optionp = comma + 1;
  } else {
    // The last suboption leaves the cursor on the terminator, so a further
    // call reports there is nothing left rather than reading this one again.
    *optionp = start + internal::string_length(start);
  }

  for (size_t i = 0; tokens[i] != nullptr; ++i) {
    size_t length = internal::string_length(tokens[i]);
    bool same = true;
    for (size_t j = 0; j < length; ++j) {
      if (start[j] != tokens[i][j]) {
        same = false;
        break;
      }
    }
    if (!same)
      continue;

    // A token matches only where the suboption ends or its value begins, so
    // that "size" is not read out of "sizeable".
    if (start[length] == '=') {
      start[length] = '\0';
      *valuep = start + length + 1;
      return static_cast<int>(i);
    }
    if (start[length] == '\0')
      return static_cast<int>(i);
  }

  // Nothing matched, so the caller is handed the whole suboption to complain
  // about, which is what the standard says the value is for here.
  *valuep = start;
  return -1;
}

} // namespace LIBC_NAMESPACE_DECL
