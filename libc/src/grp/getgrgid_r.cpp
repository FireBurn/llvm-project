//===-- Implementation of getgrgid_r --------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/grp/getgrgid_r.h"

#include "hdr/types/gid_t.h"
#include "hdr/types/size_t.h"
#include "hdr/types/struct_group.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/grp/grp_r_utils.h"

namespace LIBC_NAMESPACE_DECL {

// See the note on getgrnam_r for what makes this the reentrant form.
LLVM_LIBC_FUNCTION(int, getgrgid_r,
                   (gid_t gid, struct group *grp, char *buf, size_t buflen,
                    struct group **result)) {
  return group_r::find(grp, buf, buflen, result, [gid](struct group *entry) {
    return entry->gr_gid == gid;
  });
}

} // namespace LIBC_NAMESPACE_DECL
