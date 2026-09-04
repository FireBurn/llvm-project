//===-- Implementation of getpwuid_r --------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/pwd/getpwuid_r.h"

#include "hdr/types/size_t.h"
#include "hdr/types/struct_passwd.h"
#include "hdr/types/uid_t.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/pwd/pwd_r_utils.h"

namespace LIBC_NAMESPACE_DECL {

// See the note on getpwnam_r for what makes this the reentrant form.
LLVM_LIBC_FUNCTION(int, getpwuid_r,
                   (uid_t uid, struct passwd *pwd, char *buf, size_t buflen,
                    struct passwd **result)) {
  return passwd_r::find(pwd, buf, buflen, result, [uid](struct passwd *entry) {
    return entry->pw_uid == uid;
  });
}

} // namespace LIBC_NAMESPACE_DECL
