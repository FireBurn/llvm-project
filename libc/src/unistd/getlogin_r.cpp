//===-- Implementation of getlogin_r --------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/unistd/getlogin_r.h"

#include "hdr/errno_macros.h"
#include "hdr/types/size_t.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/pwd/getpwuid.h"
#include "src/stdlib/getenv.h"
#include "src/string/string_utils.h"
#include "src/unistd/getuid.h"

namespace LIBC_NAMESPACE_DECL {

namespace {

int copy_out(const char *name, char *buf, size_t bufsize) {
  size_t len = internal::string_length(name);
  if (len + 1 > bufsize)
    return ERANGE;
  for (size_t i = 0; i <= len; ++i)
    buf[i] = name[i];
  return 0;
}

} // anonymous namespace

// There is no utmp here to ask which name was logged in with, so the answer
// comes from LOGNAME, which the login activity is what sets, and failing
// that from the password database entry of the user the process is running
// as.
LLVM_LIBC_FUNCTION(int, getlogin_r, (char *buf, size_t bufsize)) {
  if (buf == nullptr || bufsize == 0)
    return EINVAL;

  if (const char *name = LIBC_NAMESPACE::getenv("LOGNAME");
      name != nullptr && name[0] != '\0')
    return copy_out(name, buf, bufsize);

  struct passwd *entry = LIBC_NAMESPACE::getpwuid(LIBC_NAMESPACE::getuid());
  if (entry == nullptr || entry->pw_name == nullptr)
    return ENXIO;
  return copy_out(entry->pw_name, buf, bufsize);
}

} // namespace LIBC_NAMESPACE_DECL
