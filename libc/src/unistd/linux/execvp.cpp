//===-- Linux implementation of execvp ------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/unistd/execvp.h"

#include "hdr/errno_macros.h"
#include "hdr/limits_macros.h"
#include "src/__support/OSUtil/syscall.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/stdlib/getenv.h"
#include "src/string/string_utils.h"
#include "src/unistd/environ.h"

#include <sys/syscall.h>

namespace LIBC_NAMESPACE_DECL {

namespace {

// The default used when PATH is unset. POSIX leaves this implementation
// defined; this matches what confstr(_CS_PATH) reports on Linux.
constexpr char DEFAULT_PATH[] = "/bin:/usr/bin";

int exec_at(const char *path, char *const argv[]) {
  return LIBC_NAMESPACE::syscall_impl<int>(SYS_execve, path, argv,
                                           LIBC_NAMESPACE::environ);
}

} // anonymous namespace

LLVM_LIBC_FUNCTION(int, execvp, (const char *file, char *const argv[])) {
  if (file == nullptr || *file == '\0') {
    libc_errno = ENOENT;
    return -1;
  }

  // A name containing a slash is used as given, with no search.
  if (internal::find_first_character(
          reinterpret_cast<const unsigned char *>(file), '/',
          internal::string_length(file)) != nullptr) {
    int ret = exec_at(file, argv);
    libc_errno = -ret;
    return -1;
  }

  const char *path = LIBC_NAMESPACE::getenv("PATH");
  if (path == nullptr)
    path = DEFAULT_PATH;

  const size_t file_length = internal::string_length(file);
  // Remember the most interesting failure. A later ENOENT should not mask an
  // earlier EACCES, since the caller is more likely to want to hear about a
  // file that exists but could not be run.
  bool saw_eacces = false;

  char candidate[PATH_MAX];
  for (const char *segment = path; segment != nullptr;) {
    const char *end = segment;
    while (*end != '\0' && *end != ':')
      ++end;
    const size_t dir_length = static_cast<size_t>(end - segment);
    const char *next = (*end == '\0') ? nullptr : end + 1;

    // An empty segment means the current directory. Skip anything that will
    // not fit rather than truncating it into a different path.
    const size_t prefix = (dir_length == 0) ? 1 : dir_length;
    if (prefix + 1 + file_length + 1 <= sizeof(candidate)) {
      size_t length = 0;
      if (dir_length == 0) {
        candidate[length++] = '.';
      } else {
        for (size_t i = 0; i < dir_length; ++i)
          candidate[length++] = segment[i];
      }
      candidate[length++] = '/';
      for (size_t i = 0; i < file_length; ++i)
        candidate[length++] = file[i];
      candidate[length] = '\0';

      // execve only returns on failure.
      const int error = -exec_at(candidate, argv);
      if (error == EACCES) {
        saw_eacces = true;
      } else if (error != ENOENT && error != ENOTDIR) {
        // Anything else is reported as is; searching on would hide it.
        libc_errno = error;
        return -1;
      }
    }

    segment = next;
  }

  libc_errno = saw_eacces ? EACCES : ENOENT;
  return -1;
}

} // namespace LIBC_NAMESPACE_DECL
