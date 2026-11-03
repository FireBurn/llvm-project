//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of getspnam
///
//===----------------------------------------------------------------------===//

#include "src/shadow/getspnam.h"

#include "hdr/types/struct_spwd.h"
#include "src/__support/CPP/span.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/__support/nss/lookup.h"
#include "src/__support/pwd/dynamic_buffer.h"
#include "src/shadow/shadow_r_utils.h"
#include "src/shadow/shadow_utils.h"

namespace LIBC_NAMESPACE_DECL {

namespace {

bool same(const char *a, const char *b) {
  if (a == nullptr || b == nullptr)
    return false;
  for (; *a == *b; ++a, ++b)
    if (*a == '\0')
      return true;
  return false;
}

} // anonymous namespace

LLVM_LIBC_FUNCTION(struct spwd *, getspnam, (const char *name)) {
  if (name == nullptr) {
    libc_errno = EINVAL;
    return nullptr;
  }
  static LIBC_CONSTINIT pwd::DynamicBuffer buffer;
  static struct spwd entry;
  auto found = nss::with_growing_buffer(buffer, [&](cpp::span<char> space) {
    return nss::find(
        nss::SHADOW, "getspnam_r", name, &entry, space, [&]() -> ErrorOr<bool> {
          struct spwd *match = nullptr;
          int error = shadow_r::find(
              &entry, space.data(), space.size(), &match,
              [name](struct spwd *e) { return same(e->sp_namp, name); });
          if (error != 0)
            return Error(error);
          return match != nullptr;
        });
  });
  if (!found.has_value()) {
    libc_errno = found.error();
    return nullptr;
  }
  return found.value() ? &entry : nullptr;
}

} // namespace LIBC_NAMESPACE_DECL
