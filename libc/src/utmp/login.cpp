//===-- Implementation of login ---------------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/utmp/login.h"

#include "hdr/types/struct_utmp.h"
#include "src/__support/OSUtil/syscall.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/string/memory_utils/inline_memcpy.h"
#include "src/unistd/ttyname_r.h"
#include "src/utmp/endutent.h"
#include "src/utmp/pututline.h"
#include "src/utmp/setutent.h"
#include "src/utmp/updwtmp.h"
#include "src/utmp/utmp_fill.h"

#include <sys/syscall.h>

namespace LIBC_NAMESPACE_DECL {

namespace {

// The terminal the caller is on, without the leading /dev/ that a record's
// line field does not carry. Tries the three standard descriptors, since a
// login program may have only one of them on the terminal.
bool terminal_name(char *out, size_t size) {
  char path[64];
  for (int fd = 0; fd < 3; ++fd) {
    if (LIBC_NAMESPACE::ttyname_r(fd, path, sizeof(path)) != 0)
      continue;
    const char *name = path;
    const char dev[] = "/dev/";
    size_t i = 0;
    for (; dev[i] != '\0' && name[i] == dev[i]; ++i)
      ;
    if (dev[i] == '\0')
      name += i;
    utmp_fill::field(out, size, name);
    return true;
  }
  return false;
}

} // anonymous namespace

// Records that someone has logged in, both in the table of who is logged in
// now and in the log of logins. The record the caller passes says who; the
// terminal, the process and the time are filled in here.
LLVM_LIBC_FUNCTION(void, login, (const struct utmp *entry)) {
  if (entry == nullptr)
    return;
  struct utmp record;
  inline_memcpy(&record, entry, sizeof(record));
  record.ut_type = USER_PROCESS;
  record.ut_pid =
      static_cast<pid_t>(LIBC_NAMESPACE::syscall_impl<long>(SYS_getpid));
  if (record.ut_line[0] == '\0')
    terminal_name(record.ut_line, sizeof(record.ut_line));
  utmp_fill::stamp(&record);

  LIBC_NAMESPACE::setutent();
  LIBC_NAMESPACE::pututline(&record);
  LIBC_NAMESPACE::endutent();
  LIBC_NAMESPACE::updwtmp(_PATH_WTMP, &record);
}

} // namespace LIBC_NAMESPACE_DECL
