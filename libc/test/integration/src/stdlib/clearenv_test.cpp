//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Integration tests for clearenv.
///
//===----------------------------------------------------------------------===//

#include "src/stdlib/clearenv.h"
#include "src/stdlib/getenv.h"
#include "src/stdlib/setenv.h"
#include "src/unistd/environ.h"

#include "test/IntegrationTest/test.h"

namespace LIBC_NAMESPACE {

static int count_environ() {
  if (environ == nullptr)
    return 0;
  int n = 0;
  while (environ[n] != nullptr)
    ++n;
  return n;
}

TEST_MAIN([[maybe_unused]] int argc, [[maybe_unused]] char **argv,
          [[maybe_unused]] char **envp) {
  ASSERT_EQ(setenv("CLEARENV_ONE", "first", 1), 0);
  ASSERT_EQ(setenv("CLEARENV_TWO", "second", 1), 0);
  ASSERT_TRUE(getenv("CLEARENV_ONE") != nullptr);
  ASSERT_TRUE(count_environ() > 0);

  ASSERT_EQ(clearenv(), 0);

  // Nothing is left, and there is no array either: a program which walks
  // `environ` itself has to find it null rather than empty, which is what it
  // finds elsewhere.
  ASSERT_TRUE(environ == nullptr);
  ASSERT_EQ(count_environ(), 0);
  ASSERT_TRUE(getenv("CLEARENV_ONE") == nullptr);
  ASSERT_TRUE(getenv("CLEARENV_TWO") == nullptr);

  // The environment can be built up again from nothing, which is the whole
  // point of clearing it.
  ASSERT_EQ(setenv("CLEARENV_AFTER", "yes", 1), 0);
  ASSERT_STREQ(getenv("CLEARENV_AFTER"), "yes");
  ASSERT_EQ(count_environ(), 1);

  ASSERT_EQ(clearenv(), 0);
  ASSERT_TRUE(environ == nullptr);

  return 0;
}

} // namespace LIBC_NAMESPACE
