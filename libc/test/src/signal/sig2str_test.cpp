//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the tests of sig2str and str2sig.
///
//===----------------------------------------------------------------------===//

#include "hdr/signal_macros.h"
#include "src/signal/sig2str.h"
#include "src/signal/str2sig.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcSig2strTest, NamesTheSignals) {
  char name[SIG2STR_MAX];
  ASSERT_EQ(LIBC_NAMESPACE::sig2str(SIGINT, name), 0);
  ASSERT_STREQ(name, "INT");
  ASSERT_EQ(LIBC_NAMESPACE::sig2str(SIGKILL, name), 0);
  ASSERT_STREQ(name, "KILL");
  ASSERT_EQ(LIBC_NAMESPACE::sig2str(0, name), 0);
  ASSERT_STREQ(name, "EXIT");
  ASSERT_EQ(LIBC_NAMESPACE::sig2str(SIGRTMIN, name), 0);
  ASSERT_STREQ(name, "RTMIN");
  ASSERT_EQ(LIBC_NAMESPACE::sig2str(SIGRTMAX, name), 0);
  ASSERT_STREQ(name, "RTMAX");
  ASSERT_EQ(LIBC_NAMESPACE::sig2str(SIGRTMIN + 3, name), 0);
  ASSERT_STREQ(name, "RTMIN+3");
  ASSERT_EQ(LIBC_NAMESPACE::sig2str(SIGRTMAX - 2, name), 0);
  ASSERT_STREQ(name, "RTMAX-2");

  // Between the real time signals and the others there is no signal.
  ASSERT_EQ(LIBC_NAMESPACE::sig2str(SIGSYS + 1, name), -1);
  ASSERT_EQ(LIBC_NAMESPACE::sig2str(-1, name), -1);
  ASSERT_EQ(LIBC_NAMESPACE::sig2str(NSIG, name), -1);
}

TEST(LlvmLibcSig2strTest, EveryNameIsReadBack) {
  for (int sig = 0; sig < NSIG; ++sig) {
    char name[SIG2STR_MAX];
    if (LIBC_NAMESPACE::sig2str(sig, name) != 0)
      continue;
    int back = -1;
    ASSERT_EQ(LIBC_NAMESPACE::str2sig(name, &back), 0);
    ASSERT_EQ(back, sig);
  }
}

TEST(LlvmLibcSig2strTest, Str2sigReadsNamesAndNumbers) {
  int sig = -1;
  ASSERT_EQ(LIBC_NAMESPACE::str2sig("HUP", &sig), 0);
  ASSERT_EQ(sig, int(SIGHUP));
  ASSERT_EQ(LIBC_NAMESPACE::str2sig("CLD", &sig), 0);
  ASSERT_EQ(sig, int(SIGCHLD));
  ASSERT_EQ(LIBC_NAMESPACE::str2sig("9", &sig), 0);
  ASSERT_EQ(sig, 9);
  ASSERT_EQ(LIBC_NAMESPACE::str2sig("RTMIN+1", &sig), 0);
  ASSERT_EQ(sig, SIGRTMIN + 1);
  ASSERT_EQ(LIBC_NAMESPACE::str2sig("RTMAX-1", &sig), 0);
  ASSERT_EQ(sig, SIGRTMAX - 1);

  // Not names, or beyond the range, or with the SIG that the names leave out.
  const char *const bad_names[] = {"",         "SIGHUP",   "hup",    "HUPX",
                                   "RTMIN+0",  "RTMIN-1",  "RTMIN+", "RTMAX+1",
                                   "RTMAX-99", "RTMIN+99", "65",     "-1",
                                   "1x",       "RTMIDDLE"};
  for (const char *bad : bad_names) {
    sig = 1234;
    ASSERT_EQ(LIBC_NAMESPACE::str2sig(bad, &sig), -1);
    ASSERT_EQ(sig, 1234);
  }
}
