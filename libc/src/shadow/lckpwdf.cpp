//===-- Implementation of lckpwdf -----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/shadow/lckpwdf.h"

#include "hdr/fcntl_macros.h"
#include "hdr/stdio_macros.h"
#include "hdr/types/struct_flock.h"
#include "src/__support/OSUtil/syscall.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/shadow/pwdlock.h"
#include <sys/syscall.h>

namespace LIBC_NAMESPACE_DECL {

namespace shadow_lock {
int held_fd = -1;
} // namespace shadow_lock

// How long to keep trying for, which is what every other implementation
// waits.
static constexpr int TIMEOUT_SECONDS = 15;

LLVM_LIBC_FUNCTION(int, lckpwdf, (void)) {
  // Only one lock at a time, and asking twice is an error rather than a
  // second lock to release.
  if (shadow_lock::held_fd >= 0)
    return -1;

  int fd = LIBC_NAMESPACE::syscall_impl<int>(
      SYS_openat, AT_FDCWD, shadow_lock::LOCK_PATH,
      O_WRONLY | O_CREAT | O_CLOEXEC, 0600);
  if (fd < 0)
    return -1;

  struct flock fl;
  fl.l_type = F_WRLCK;
  fl.l_whence = SEEK_SET;
  fl.l_start = 0;
  fl.l_len = 0;

  // The wait is spent asking again rather than blocking in the kernel, so
  // that it can be given up on after the timeout.
  for (int waited = 0; waited <= TIMEOUT_SECONDS; ++waited) {
    if (LIBC_NAMESPACE::syscall_impl<int>(SYS_fcntl, fd, F_SETLK, &fl) >= 0) {
      shadow_lock::held_fd = fd;
      return 0;
    }
    if (waited == TIMEOUT_SECONDS)
      break;
    struct {
      long tv_sec;
      long tv_nsec;
    } one_second = {1, 0};
    LIBC_NAMESPACE::syscall_impl<int>(SYS_nanosleep, &one_second, nullptr);
  }

  LIBC_NAMESPACE::syscall_impl<int>(SYS_close, fd);
  return -1;
}

} // namespace LIBC_NAMESPACE_DECL
