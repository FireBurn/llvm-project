//===-- Where the zone is read from -----------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_TIME_TZ_ZONE_SOURCE_H
#define LLVM_LIBC_SRC_TIME_TZ_ZONE_SOURCE_H

#include "hdr/types/size_t.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {
namespace tz {

// The bytes a zone was stored as, held where they are for as long as anything
// reads them.
struct Mapping {
  void *data = nullptr;
  size_t size = 0;
};

// Reads the whole of a file. Nothing here goes through malloc: this runs
// before the allocator is ready, and what it hands back has to outlast any
// allocator the program later tears down.
//
// Returns false where the file cannot be read, and on a target with no files
// to read.
bool map_zone_file(const char *path, Mapping &out);

void unmap_zone_file(Mapping &mapping);

// What TZ says, or nullptr where it says nothing. getenv is not used because
// this runs from the startup path, before anything has set up more than the
// environment itself.
const char *tz_from_environment();

} // namespace tz
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_TIME_TZ_ZONE_SOURCE_H
