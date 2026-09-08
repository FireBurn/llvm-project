//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// The parse the reentrant gshadow lookups share.
///
/// A struct sgrp holds two arrays of pointers, and the caller's buffer is
/// the only place they can go. The buffer is split into a part for the line
/// and a part for the two pointer arrays.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_GSHADOW_GSHADOW_R_UTILS_H
#define LLVM_LIBC_SRC_GSHADOW_GSHADOW_R_UTILS_H

#include "hdr/errno_macros.h"
#include "hdr/types/size_t.h"
#include "hdr/types/struct_sgrp.h"
#include "src/__support/CPP/span.h"
#include "src/__support/macros/attributes.h"
#include "src/__support/macros/config.h"
#include "src/gshadow/gshadow_utils.h"

namespace LIBC_NAMESPACE_DECL {
namespace gshadow_r {

// How the caller's buffer is divided. The last quarter holds the two
// pointer arrays, the rest holds the line.
struct Split {
  cpp::span<char> line;
  char **admins;
  size_t max_admins;
  char **members;
  size_t max_members;
};

// Returns false when the buffer is too small to be worth trying with.
LIBC_INLINE bool split_buffer(char *buf, size_t buflen, Split *out) {
  // Room for the line, plus at least the two terminating nulls of the
  // arrays, aligned for a pointer.
  constexpr size_t MIN_POINTERS = 2;
  size_t reserved = buflen / 4;
  if (reserved < MIN_POINTERS * sizeof(char *) + alignof(char *))
    return false;
  size_t line_len = buflen - reserved;

  // The arrays start at the first aligned address past the line.
  size_t offset = line_len;
  size_t misalign = reinterpret_cast<uintptr_t>(buf + offset) % alignof(char *);
  if (misalign != 0)
    offset += alignof(char *) - misalign;
  if (offset >= buflen)
    return false;

  size_t pointers = (buflen - offset) / sizeof(char *);
  if (pointers < MIN_POINTERS)
    return false;

  out->line = cpp::span<char>(buf, line_len);
  out->admins = reinterpret_cast<char **>(buf + offset);
  out->max_admins = pointers / 2;
  out->members = out->admins + out->max_admins;
  out->max_members = pointers - out->max_admins;
  return true;
}

} // namespace gshadow_r
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_GSHADOW_GSHADOW_R_UTILS_H
