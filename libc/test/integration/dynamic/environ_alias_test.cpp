//===-- Test of _environ and __environ -------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Tests that the library exports _environ and __environ as the same variable
/// as environ, since binaries built against glibc refer to it by either name
/// and fail to load without them. The program returns nonzero if any check
/// fails.
///
//===----------------------------------------------------------------------===//

#include "check.h"

extern "C" {
extern char **environ;
extern char **_environ;
extern char **__environ;
}

extern "C" int main(int, char **, char **envp) {
  // Taken through volatile, since the compiler takes two different names for
  // two different variables and folds the comparison away.
  char ***volatile plain = &environ;
  char ***volatile single = &_environ;
  char ***volatile double_ = &__environ;
  CHECK(single == plain);
  CHECK(double_ == plain);
  // The variable holds the environment the program was started with.
  CHECK(environ == envp);
  CHECK(_environ == envp);
  CHECK(__environ == envp);
  return failures == 0 ? 0 : 1;
}
