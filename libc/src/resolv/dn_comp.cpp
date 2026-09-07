//===-- Implementation of dn_comp -----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/resolv/dn_comp.h"

#include "src/__support/common.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

namespace {

// The longest one part of a name, and the longest name once written out.
constexpr int MAX_LABEL = 63;
constexpr int MAX_NAME = 255;

} // anonymous namespace

// Writes `src` into `dst` in the form a message carries it: each part of the
// name preceded by its length, and a zero byte at the end. Returns how many
// bytes that took.
//
// `dnptrs` records where names have already been written so that a later one
// can point at an earlier rather than repeat it. Nothing is asked to be
// written twice here, so the list is left as it was found; that is allowed,
// since pointing back is an economy rather than a requirement.
LLVM_LIBC_FUNCTION(int, dn_comp,
                   (const char *src, unsigned char *dst, int dstsiz,
                    unsigned char **dnptrs, unsigned char **lastdnptr)) {
  (void)dnptrs;
  (void)lastdnptr;
  if (src == nullptr || dst == nullptr || dstsiz <= 0)
    return -1;

  int written = 0;
  int total = 0;
  const char *at = src;

  for (;;) {
    // How long this part of the name is, up to the next dot or the end.
    int label = 0;
    while (at[label] != '\0' && at[label] != '.')
      ++label;

    if (label > MAX_LABEL)
      return -1;
    if (label == 0) {
      // A dot with nothing before it is only allowed as the root at the end.
      if (*at == '.' && at[1] == '\0')
        ++at;
      break;
    }

    total += label + 1;
    if (total > MAX_NAME)
      return -1;
    if (written + label + 1 >= dstsiz)
      return -1;

    dst[written++] = static_cast<unsigned char>(label);
    for (int i = 0; i < label; ++i)
      dst[written++] = static_cast<unsigned char>(at[i]);

    at += label;
    if (*at == '\0')
      break;
    ++at; // Step over the dot.
    if (*at == '\0')
      break;
  }

  if (written >= dstsiz)
    return -1;
  dst[written++] = 0;
  return written;
}

} // namespace LIBC_NAMESPACE_DECL
