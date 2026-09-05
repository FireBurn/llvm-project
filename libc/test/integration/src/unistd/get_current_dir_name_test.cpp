//===-- Integration test for get_current_dir_name -------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/stdlib/setenv.h"
#include "src/stdlib/unsetenv.h"
#include "src/sys/stat/stat.h"
#include "src/unistd/get_current_dir_name.h"
#include "test/IntegrationTest/test.h"

#include <stdlib.h> // For free

// The name comes back in memory the caller frees, and names the directory
// the process is in whether or not PWD says so.
void names_the_working_directory() {
  ASSERT_EQ(LIBC_NAMESPACE::unsetenv("PWD"), 0);

  char *name = LIBC_NAMESPACE::get_current_dir_name();
  ASSERT_TRUE(name != nullptr);

  struct stat st_dot;
  struct stat st_name;
  ASSERT_EQ(LIBC_NAMESPACE::stat(".", &st_dot), 0);
  ASSERT_EQ(LIBC_NAMESPACE::stat(name, &st_name), 0);
  ASSERT_EQ(st_dot.st_dev, st_name.st_dev);
  ASSERT_EQ(st_dot.st_ino, st_name.st_ino);
  free(name);
}

// PWD is taken where it is absolute and still names the same directory,
// since it keeps the symbolic links the user walked through.
void prefers_pwd_where_it_still_names_the_place() {
  char *resolved = LIBC_NAMESPACE::get_current_dir_name();
  ASSERT_TRUE(resolved != nullptr);
  ASSERT_EQ(LIBC_NAMESPACE::setenv("PWD", resolved, 1), 0);

  char *name = LIBC_NAMESPACE::get_current_dir_name();
  ASSERT_TRUE(name != nullptr);

  struct stat st_pwd;
  struct stat st_name;
  ASSERT_EQ(LIBC_NAMESPACE::stat(resolved, &st_pwd), 0);
  ASSERT_EQ(LIBC_NAMESPACE::stat(name, &st_name), 0);
  ASSERT_EQ(st_pwd.st_dev, st_name.st_dev);
  ASSERT_EQ(st_pwd.st_ino, st_name.st_ino);
  free(name);
  free(resolved);
}

// A PWD naming somewhere else is ignored rather than believed.
void ignores_pwd_that_names_somewhere_else() {
  ASSERT_EQ(LIBC_NAMESPACE::setenv("PWD", "/", 1), 0);

  char *name = LIBC_NAMESPACE::get_current_dir_name();
  ASSERT_TRUE(name != nullptr);

  struct stat st_dot;
  struct stat st_name;
  ASSERT_EQ(LIBC_NAMESPACE::stat(".", &st_dot), 0);
  ASSERT_EQ(LIBC_NAMESPACE::stat(name, &st_name), 0);
  ASSERT_EQ(st_dot.st_dev, st_name.st_dev);
  ASSERT_EQ(st_dot.st_ino, st_name.st_ino);
  free(name);
}

TEST_MAIN([[maybe_unused]] int argc, [[maybe_unused]] char **argv,
          [[maybe_unused]] char **envp) {
  names_the_working_directory();
  prefers_pwd_where_it_still_names_the_place();
  ignores_pwd_that_names_somewhere_else();
  return 0;
}
