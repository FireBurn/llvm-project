//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Shared helpers for the mntent family.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_MNTENT_MNTENT_UTILS_H
#define LLVM_LIBC_SRC_MNTENT_MNTENT_UTILS_H

#include "hdr/types/struct_mntent.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {
namespace mntent {

// The buffer getmntent hands to getmntent_r. A mount table line is bounded
// by the kernel at well under this.
constexpr int LINE_SIZE = 4096;

// Fills |entry| from one line of a mount table, which |line| is allowed to
// modify. Returns false for a blank or comment line, or one with too few
// fields.
bool parse_line(char *line, struct mntent *entry);

// Appends |entry| to the mount table open on |f| as one line, with the
// characters which would otherwise separate fields written as their octal
// escapes. Returns false on a write error.
bool write_entry(void *f, const struct mntent *entry);

} // namespace mntent
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_MNTENT_MNTENT_UTILS_H
