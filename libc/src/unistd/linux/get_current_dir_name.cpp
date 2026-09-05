//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Linux implementation of get_current_dir_name
///
//===----------------------------------------------------------------------===//

#include "src/unistd/get_current_dir_name.h"

#include "hdr/errno_macros.h"
#include "hdr/fcntl_macros.h"
#include "hdr/limits_macros.h"
#include "hdr/types/struct_stat.h"
#include "src/__support/CPP/optional.h"
#include "src/__support/OSUtil/linux/stat/stat_via_statx.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/getcwd.h"
#include "src/__support/common.h"
#include "src/__support/error_or.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/stdlib/environ_internal.h"
#include "src/string/allocating_string_utils.h" // For strdup.

namespace LIBC_NAMESPACE_DECL {

namespace {

// Whether the path names the directory the program is in. Two entries are
// the same file when they are on the same device with the same inode.
bool names_current_directory(const char *path) {
  struct stat by_name;
  struct stat here;
  if (!internal::stat_via_statx(AT_FDCWD, path, 0, &by_name))
    return false;
  if (!internal::stat_via_statx(AT_FDCWD, ".", 0, &here))
    return false;
  return by_name.st_dev == here.st_dev && by_name.st_ino == here.st_ino;
}

} // namespace

LLVM_LIBC_FUNCTION(char *, get_current_dir_name, (void)) {
  // PWD is what the shell put there, so it keeps the symlinks the user
  // walked through rather than the path they resolve to. It is only worth
  // anything if it is absolute and still names where the program is.
  const char *pwd = internal::EnvironmentManager::get_instance().get("PWD");
  if (pwd != nullptr && pwd[0] == '/' && names_current_directory(pwd)) {
    cpp::optional<char *> copy = internal::strdup(pwd);
    if (copy)
      return *copy;
    libc_errno = ENOMEM;
    return nullptr;
  }

  char pathbuf[PATH_MAX];
  ErrorOr<ssize_t> written = linux_syscalls::getcwd(pathbuf, PATH_MAX);
  if (!written) {
    libc_errno = written.error();
    return nullptr;
  }
  cpp::optional<char *> cwd = internal::strdup(pathbuf);
  if (!cwd) {
    libc_errno = ENOMEM;
    return nullptr;
  }
  return *cwd;
}

} // namespace LIBC_NAMESPACE_DECL
