//===-- Where the zone is read from, on a target with nowhere to read it --===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/time/tz/zone_source.h"

#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {
namespace tz {

bool map_zone_file(const char *, Mapping &) { return false; }

void unmap_zone_file(Mapping &) {}

const char *tz_from_environment() { return nullptr; }

} // namespace tz
} // namespace LIBC_NAMESPACE_DECL
