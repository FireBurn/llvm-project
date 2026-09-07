//===-- Linux implementation of getpass -----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/unistd/getpass.h"

#include "hdr/fcntl_macros.h"
#include "hdr/termios_macros.h"
#include "hdr/types/struct_termios.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/open.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/read.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/write.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/termios/tcgetattr.h"
#include "src/termios/tcsetattr.h"
#include "src/unistd/close.h"

namespace LIBC_NAMESPACE_DECL {

namespace {

// What is typed is kept in storage the library owns, since the interface
// hands back a pointer and no length. A longer line is cut short here rather
// than left for whatever reads the terminal next.
constexpr size_t PASSPHRASE_SIZE = 128;

char passphrase[PASSPHRASE_SIZE];

void say(int fd, const char *text) {
  if (text == nullptr)
    return;
  size_t length = 0;
  while (text[length] != '\0')
    ++length;
  size_t written = 0;
  while (written < length) {
    auto taken = linux_syscalls::write(fd, text + written, length - written);
    if (!taken.has_value() || taken.value() == 0)
      return;
    written += static_cast<size_t>(taken.value());
  }
}

} // anonymous namespace

// The prompt goes to the terminal and the answer comes from it, not from the
// standard streams, so that neither can be redirected to put a passphrase
// somewhere it should not go. Where there is no terminal, the standard input
// and standard error stand in, as they always have.
LLVM_LIBC_FUNCTION(char *, getpass, (const char *prompt)) {
  int in = -1;
  int out = -1;
  bool own_fd = false;

  auto tty = linux_syscalls::open("/dev/tty", O_RDWR | O_CLOEXEC | O_NOCTTY, 0);
  if (tty.has_value()) {
    in = out = tty.value();
    own_fd = true;
  } else {
    in = 0;
    out = 2;
  }

  // Echo is turned off for as long as the answer is being typed, and put back
  // however this leaves. ECHONL is left on so that the newline the user types
  // still moves the cursor down.
  struct termios before;
  const bool have_terminal = LIBC_NAMESPACE::tcgetattr(in, &before) == 0;
  if (have_terminal) {
    struct termios quiet = before;
    quiet.c_lflag &= static_cast<decltype(quiet.c_lflag)>(~ECHO);
    quiet.c_lflag |= ECHONL;
    // TCSAFLUSH so that anything typed ahead is thrown away rather than
    // echoed once the terminal is itself again.
    if (LIBC_NAMESPACE::tcsetattr(in, TCSAFLUSH, &quiet) != 0)
      libc_errno = 0;
  }

  say(out, prompt);

  size_t length = 0;
  bool ended = false;
  int error = 0;
  while (!ended) {
    char c;
    auto taken = linux_syscalls::read(in, &c, 1);
    if (!taken.has_value()) {
      error = taken.error();
      break;
    }
    if (taken.value() == 0 || c == '\n')
      ended = true;
    else if (length + 1 < PASSPHRASE_SIZE)
      passphrase[length++] = c;
    // Anything past the end is read and dropped, so that the rest of the line
    // is not left behind for the next thing to read the terminal.
  }
  passphrase[length] = '\0';

  if (have_terminal) {
    LIBC_NAMESPACE::tcsetattr(in, TCSAFLUSH, &before);
    // The newline the user typed was swallowed with the echo, so one is
    // written to put the cursor where it would have been.
    say(out, "\n");
  }

  if (own_fd)
    LIBC_NAMESPACE::close(in);

  if (error != 0) {
    libc_errno = error;
    return nullptr;
  }
  return passphrase;
}

} // namespace LIBC_NAMESPACE_DECL
