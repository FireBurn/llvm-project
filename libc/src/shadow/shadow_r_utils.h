//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// The search the reentrant shadow lookups share.
///
/// Neither may touch anything another thread might be using, so each opens
/// the file for itself and parses into the caller's buffer rather than into
/// the one getspent iterates with.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_SHADOW_SHADOW_R_UTILS_H
#define LLVM_LIBC_SRC_SHADOW_SHADOW_R_UTILS_H

#include "hdr/errno_macros.h"
#include "hdr/types/size_t.h"
#include "hdr/types/struct_spwd.h"
#include "src/__support/CPP/span.h"
#include "src/__support/flat_file_db.h"
#include "src/__support/macros/attributes.h"
#include "src/__support/macros/config.h"
#include "src/shadow/shadow_utils.h"

namespace LIBC_NAMESPACE_DECL {
namespace shadow_r {

template <typename Match>
LIBC_INLINE int find(struct spwd *entry, char *buffer, size_t buflen,
                     struct spwd **result, Match matches) {
  if (result != nullptr)
    *result = nullptr;
  if (entry == nullptr || buffer == nullptr || result == nullptr)
    return EINVAL;
  if (buflen < 2)
    return ERANGE;

  internal::FlatFileDatabase<struct spwd> db(shadow_db::path());
  if (auto opened = db.setdb(); !opened.has_value())
    return opened.error();

  for (;;) {
    auto read = db.getnext(entry, cpp::span<char>(buffer, buflen));
    if (!read.has_value()) {
      db.enddb();
      // A line too long for the buffer is what ERANGE is for; the caller
      // enlarges it and asks again.
      return read.error();
    }
    if (!read.value())
      break; // The end of the file, and no match is not an error.
    if (matches(entry)) {
      db.enddb();
      *result = entry;
      return 0;
    }
  }
  db.enddb();
  return 0;
}

} // namespace shadow_r
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_SHADOW_SHADOW_R_UTILS_H
