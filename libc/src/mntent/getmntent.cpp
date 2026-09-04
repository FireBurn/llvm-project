//===-- Implementation of getmntent
//----------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/mntent/getmntent.h"

#include "hdr/types/FILE.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/mntent/getmntent_r.h"
#include "src/mntent/mntent_utils.h"

namespace LIBC_NAMESPACE_DECL {

namespace {

// getmntent is not reentrant: the entry it returns points into a buffer
// shared by every caller in the process.
struct mntent entry;
char buffer[mntent::LINE_SIZE];

} // anonymous namespace

LLVM_LIBC_FUNCTION(struct mntent *, getmntent, (::FILE * stream)) {
  return LIBC_NAMESPACE::getmntent_r(stream, &entry, buffer, mntent::LINE_SIZE);
}

} // namespace LIBC_NAMESPACE_DECL
