//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Integration test for reading limits under a sandbox
///
//===----------------------------------------------------------------------===//

#include "hdr/types/struct_rlimit.h"
#include "src/__support/OSUtil/syscall.h"
#include "src/sys/resource/getrlimit.h"
#include "src/sys/resource/setrlimit.h"
#include "src/unistd/sysconf.h"
#include "test/IntegrationTest/test.h"

#include <linux/filter.h>
#include <linux/prctl.h>
#include <linux/seccomp.h>
#include <sys/resource.h>
#include <sys/syscall.h>
#include <unistd.h>

// Firefox's and Chromium's sandboxes let a process read its own limits with
// getrlimit and kill it for prlimit64, which reaches other processes.

static void refuse(long number) {
  sock_filter filter[] = {
      BPF_STMT(BPF_LD | BPF_W | BPF_ABS, __builtin_offsetof(seccomp_data, nr)),
      BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, static_cast<unsigned>(number), 0, 1),
      BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_KILL_PROCESS),
      BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
  };
  sock_fprog program = {sizeof(filter) / sizeof(filter[0]), filter};
  ASSERT_EQ(LIBC_NAMESPACE::syscall_impl<long>(SYS_prctl, PR_SET_NO_NEW_PRIVS,
                                               1, 0, 0, 0),
            0L);
  ASSERT_EQ(LIBC_NAMESPACE::syscall_impl<long>(
                SYS_seccomp, SECCOMP_SET_MODE_FILTER, 0, &program),
            0L);
}

TEST_MAIN() {
  refuse(SYS_prlimit64);

  struct rlimit limit;
  ASSERT_EQ(LIBC_NAMESPACE::getrlimit(RLIMIT_CORE, &limit), 0);
  // Setting a limit to what it already is needs no privilege.
  ASSERT_EQ(LIBC_NAMESPACE::setrlimit(RLIMIT_CORE, &limit), 0);
  ASSERT_TRUE(LIBC_NAMESPACE::sysconf(_SC_OPEN_MAX) != 0);
  ASSERT_TRUE(LIBC_NAMESPACE::sysconf(_SC_CHILD_MAX) != 0);
  ASSERT_TRUE(LIBC_NAMESPACE::sysconf(_SC_ARG_MAX) != 0);
  return 0;
}
