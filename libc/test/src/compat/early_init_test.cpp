//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for __llvm_libc_early_init
///
//===----------------------------------------------------------------------===//

#include "src/compat/__llvm_libc_early_init.h"
#include "src/errno/program_invocation_name.h"
#include "src/errno/program_invocation_short_name.h"
#include "src/pthread/pthread_self.h"
#include "src/unistd/environ.h"
#include "test/UnitTest/Test.h"

namespace {

// Puts back what the call changed, so the rest of the process is unaffected.
struct Restore {
  char **environment = LIBC_NAMESPACE::environ;
  char *name = LIBC_NAMESPACE::program_invocation_name;
  char *short_name = LIBC_NAMESPACE::program_invocation_short_name;
  ~Restore() {
    LIBC_NAMESPACE::environ = environment;
    LIBC_NAMESPACE::program_invocation_name = name;
    LIBC_NAMESPACE::program_invocation_short_name = short_name;
  }
};

} // namespace

TEST(LlvmLibcEarlyInitTest, TakesTheEnvironmentAndTheProgramName) {
  Restore restore;
  char program[] = "/usr/bin/prog";
  char variable[] = "FIRST=1";
  char *argv[] = {program, nullptr};
  char *envp[] = {variable, nullptr};

  ASSERT_EQ(LIBC_NAMESPACE::__llvm_libc_early_init(1, argv, envp, nullptr), 0);

  ASSERT_TRUE(LIBC_NAMESPACE::environ == envp);
  ASSERT_TRUE(LIBC_NAMESPACE::program_invocation_name == program);
  // The short name is the last component.
  ASSERT_TRUE(LIBC_NAMESPACE::program_invocation_short_name == program + 9);
}

// A loader may have nothing to say about some of it, and a second call must
// not undo the first.
TEST(LlvmLibcEarlyInitTest, LeavesWhatIsNotGivenAlone) {
  Restore restore;
  char program[] = "prog";
  char *argv[] = {program, nullptr};
  char *envp[] = {nullptr};
  ASSERT_EQ(LIBC_NAMESPACE::__llvm_libc_early_init(1, argv, envp, nullptr), 0);

  ASSERT_EQ(LIBC_NAMESPACE::__llvm_libc_early_init(0, nullptr, nullptr, nullptr),
            0);
  ASSERT_TRUE(LIBC_NAMESPACE::environ == envp);
  ASSERT_TRUE(LIBC_NAMESPACE::program_invocation_name == program);
}

// An initialiser may ask which thread it is on, and a thread libc has not
// described has nothing to answer with.
TEST(LlvmLibcEarlyInitTest, DescribesTheMainThread) {
  Restore restore;
  ASSERT_EQ(LIBC_NAMESPACE::__llvm_libc_early_init(0, nullptr, nullptr, nullptr),
            0);
  ASSERT_TRUE(LIBC_NAMESPACE::pthread_self() != 0);
}
