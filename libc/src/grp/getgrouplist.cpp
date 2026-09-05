//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of getgrouplist.
///
//===----------------------------------------------------------------------===//

#include "src/grp/getgrouplist.h"
#include "hdr/errno_macros.h"
#include "hdr/types/gid_t.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/__support/macros/null_check.h"
#include "src/grp/grp_utils.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, getgrouplist,
                   (const char *user, gid_t group, gid_t *groups,
                    int *ngroups)) {
  LIBC_CRASH_ON_NULLPTR(user);
  LIBC_CRASH_ON_NULLPTR(ngroups);
  if (*ngroups > 0)
    LIBC_CRASH_ON_NULLPTR(groups);

  if (*ngroups < 0) {
    libc_errno = EINVAL;
    return -1;
  }

  const auto res =
      grp::get_group_list(user, group, groups, static_cast<size_t>(*ngroups));
  if (!res.has_value()) {
    libc_errno = res.error();
    return -1;
  }

  const int total = static_cast<int>(res.value());
  const int requested = *ngroups;
  *ngroups = total;

  if (requested < total)
    return -1;

  return total;
}

} // namespace LIBC_NAMESPACE_DECL
//===-- Implementation of getgrouplist ------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/grp/getgrouplist.h"

#include "hdr/types/gid_t.h"
#include "hdr/types/struct_group.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/grp/grp_utils.h"

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

// The groups a user belongs to: the one passed in, which is the login group
// and comes first, and every group whose member list names the user.
//
// ngroups says how much room there is on the way in and how many groups
// there are on the way out, which may be more than would fit. A caller
// which was given too little room enlarges the array and asks again.
LLVM_LIBC_FUNCTION(int, getgrouplist,
                   (const char *user, gid_t group, gid_t *groups,
                    int *ngroups)) {
  if (user == nullptr || ngroups == nullptr)
    return -1;

  const int room = *ngroups;
  int found = 0;

  auto add = [&](gid_t gid) {
    for (int i = 0; i < found && i < room; ++i)
      if (groups[i] == gid)
        return; // Already there.
    if (found < room && groups != nullptr)
      groups[found] = gid;
    ++found;
  };

  add(group);

  if (auto opened = grp::open(); opened.has_value()) {
    for (;;) {
      auto entry = grp::read_next();
      if (!entry.has_value() || entry.value() == nullptr)
        break;
      const struct group *g = entry.value();
      for (size_t i = 0; g->gr_mem != nullptr && g->gr_mem[i] != nullptr; ++i) {
        if (same(g->gr_mem[i], user)) {
          add(g->gr_gid);
          break;
        }
      }
    }
    grp::close();
  }

  *ngroups = found;
  return found <= room ? found : -1;
}

} // namespace LIBC_NAMESPACE_DECL
