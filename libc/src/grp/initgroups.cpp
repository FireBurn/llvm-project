//===-- Implementation of initgroups --------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/grp/initgroups.h"

#include "hdr/errno_macros.h"
#include "hdr/limits_macros.h"
#include "hdr/types/gid_t.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/grp/grp_utils.h"
#include "src/grp/setgroups.h"
#include "src/string/string_utils.h"

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

// A process cannot hold more than this many groups, so gathering more than
// this from the database would be wasted.
constexpr size_t MAX_GROUPS = 64;

} // anonymous namespace

LLVM_LIBC_FUNCTION(int, initgroups, (const char *user, gid_t group)) {
  if (user == nullptr) {
    libc_errno = EINVAL;
    return -1;
  }

  gid_t list[MAX_GROUPS];
  size_t count = 0;
  // The group given by the caller is always in the list, whether or not the
  // database names the user as a member of it.
  list[count++] = group;

  if (auto opened = group::open(); !opened.has_value()) {
    libc_errno = opened.error();
    return -1;
  }
  for (;;) {
    auto entry = group::read_next();
    if (!entry.has_value()) {
      libc_errno = entry.error();
      group::close();
      return -1;
    }
    if (entry.value() == nullptr)
      break;

    struct group *g = entry.value();
    for (char **member = g->gr_mem; member != nullptr && *member != nullptr;
         ++member) {
      if (!same(*member, user))
        continue;
      bool already = false;
      for (size_t i = 0; i < count; ++i)
        already = already || list[i] == g->gr_gid;
      if (!already && count < MAX_GROUPS)
        list[count++] = g->gr_gid;
      break;
    }
  }
  group::close();

  return LIBC_NAMESPACE::setgroups(count, list);
}

} // namespace LIBC_NAMESPACE_DECL
