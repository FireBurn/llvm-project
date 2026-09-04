//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the public paths.h header.
///
//===----------------------------------------------------------------------===//

#include "include/paths.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcPathsTest, StringMacros) {
  EXPECT_STREQ(_PATH_DEFPATH, "/usr/bin:/bin");
  EXPECT_STREQ(_PATH_STDPATH, "/usr/bin:/bin:/usr/sbin:/sbin");
  EXPECT_STREQ(_PATH_BSHELL, "/bin/sh");
  EXPECT_STREQ(_PATH_CSHELL, "/bin/csh");
  EXPECT_STREQ(_PATH_CONSOLE, "/dev/console");
  EXPECT_STREQ(_PATH_DEVNULL, "/dev/null");
  EXPECT_STREQ(_PATH_KLOG, "/proc/kmsg");
  EXPECT_STREQ(_PATH_KMEM, "/dev/kmem");
  EXPECT_STREQ(_PATH_MEM, "/dev/mem");
  EXPECT_STREQ(_PATH_TTY, "/dev/tty");
  EXPECT_STREQ(_PATH_UNIX, "/boot/vmlinux");
  EXPECT_STREQ(_PATH_GSHADOW, "/etc/gshadow");
  EXPECT_STREQ(_PATH_SHADOW, "/etc/shadow");
  EXPECT_STREQ(_PATH_SHELLS, "/etc/shells");
  EXPECT_STREQ(_PATH_NOLOGIN, "/etc/nologin");
  EXPECT_STREQ(_PATH_MNTTAB, "/etc/fstab");
  EXPECT_STREQ(_PATH_MOUNTED, "/etc/mtab");
  EXPECT_STREQ(_PATH_LASTLOG, "/var/log/lastlog");
  EXPECT_STREQ(_PATH_MAILDIR, "/var/mail");
  EXPECT_STREQ(_PATH_MAN, "/usr/share/man");
  EXPECT_STREQ(_PATH_PRESERVE, "/var/lib");
  EXPECT_STREQ(_PATH_SENDMAIL, "/usr/sbin/sendmail");
  EXPECT_STREQ(_PATH_UTMP, "/var/run/utmp");
  EXPECT_STREQ(_PATH_WTMP, "/var/log/wtmp");
  EXPECT_STREQ(_PATH_VI, "/usr/bin/vi");
  EXPECT_STREQ(_PATH_DEV, "/dev/");
  EXPECT_STREQ(_PATH_TMP, "/tmp/");
  EXPECT_STREQ(_PATH_VARDB, "/var/db/");
  EXPECT_STREQ(_PATH_VARRUN, "/var/run/");
  EXPECT_STREQ(_PATH_VARTMP, "/var/tmp/");
}
