//===-- Implementation of popen -------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/stdio/popen.h"

#include "hdr/errno_macros.h"
#include "hdr/types/FILE.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/stdio/fclose.h"
#include "src/stdio/fdopen.h"
#include "src/stdio/linux/popen_registry.h"
#include "src/unistd/_exit.h"
#include "src/unistd/close.h"
#include "src/unistd/dup2.h"
#include "src/unistd/execv.h"
#include "src/unistd/fork.h"
#include "src/unistd/pipe.h"

namespace LIBC_NAMESPACE_DECL {

namespace {

// The exit status POSIX gives for a command that could not be executed.
constexpr int EXEC_FAILED = 127;

} // anonymous namespace

LLVM_LIBC_FUNCTION(::FILE *, popen, (const char *command, const char *type)) {
  if (command == nullptr || type == nullptr) {
    libc_errno = EINVAL;
    return nullptr;
  }
  // POSIX only requires "r" and "w"; anything else is an error rather than
  // being silently treated as one of them.
  const bool reading = type[0] == 'r';
  if ((!reading && type[0] != 'w') || type[1] != '\0') {
    libc_errno = EINVAL;
    return nullptr;
  }

  int fds[2];
  if (LIBC_NAMESPACE::pipe(fds) != 0)
    return nullptr;

  // The parent keeps one end and the child inherits the other.
  const int parent_fd = reading ? fds[0] : fds[1];
  const int child_fd = reading ? fds[1] : fds[0];

  pid_t pid = LIBC_NAMESPACE::fork();
  if (pid < 0) {
    LIBC_NAMESPACE::close(fds[0]);
    LIBC_NAMESPACE::close(fds[1]);
    return nullptr;
  }

  if (pid == 0) {
    // The child does not need the parent's end, and leaving it open would
    // stop the parent ever seeing end of file.
    LIBC_NAMESPACE::close(parent_fd);
    if (LIBC_NAMESPACE::dup2(child_fd, reading ? 1 : 0) < 0)
      LIBC_NAMESPACE::_exit(EXEC_FAILED);
    LIBC_NAMESPACE::close(child_fd);
    char *const argv[] = {const_cast<char *>("sh"), const_cast<char *>("-c"),
                          const_cast<char *>(command), nullptr};
    LIBC_NAMESPACE::execv("/bin/sh", argv);
    // execv only returns on failure.
    LIBC_NAMESPACE::_exit(EXEC_FAILED);
  }

  LIBC_NAMESPACE::close(child_fd);
  ::FILE *stream = LIBC_NAMESPACE::fdopen(parent_fd, reading ? "r" : "w");
  if (stream == nullptr) {
    LIBC_NAMESPACE::close(parent_fd);
    return nullptr;
  }
  if (!internal::popen_remember(stream, pid)) {
    // Without a record of the child, pclose could never reap it.
    LIBC_NAMESPACE::fclose(stream);
    libc_errno = EMFILE;
    return nullptr;
  }
  return stream;
}

} // namespace LIBC_NAMESPACE_DECL
