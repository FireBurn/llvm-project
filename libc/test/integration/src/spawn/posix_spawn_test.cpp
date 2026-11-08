//===-- Unittests for posix_spawn -----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "test_binary_properties.h"

#include "hdr/errno_macros.h"
#include "hdr/spawn_macros.h"
#include "hdr/stdint_proxy.h"
#include "src/fcntl/open.h"
#include "src/spawn/posix_spawn.h"
#include "src/spawn/posix_spawn_file_actions_adddup2.h"
#include "src/spawn/posix_spawn_file_actions_addopen.h"
#include "src/spawn/posix_spawn_file_actions_destroy.h"
#include "src/spawn/posix_spawn_file_actions_init.h"
#include "src/spawn/posix_spawnattr_destroy.h"
#include "src/spawn/posix_spawnattr_init.h"
#include "src/spawn/posix_spawnattr_setflags.h"
#include "src/spawn/posix_spawnp.h"
#include "src/sys/wait/waitpid.h"
#include "src/unistd/close.h"
#include "src/unistd/dup3.h"
#include "test/IntegrationTest/test.h"

#include <fcntl.h>
#include <spawn.h>
#include <stddef.h>
#include <sys/wait.h>

char arg0[] = "libc_posix_spawn_test_binary";
char *argv[] = {
    arg0,
    nullptr,
};

void spawn_and_wait_for_normal_exit(char **envp) {
  pid_t cpid;
  posix_spawn_file_actions_t file_actions;
  ASSERT_EQ(LIBC_NAMESPACE::posix_spawn_file_actions_init(&file_actions), 0);
  LIBC_NAMESPACE::posix_spawn_file_actions_addopen(
      &file_actions, CHILD_FD, "testdata/posix_spawn.test", O_RDONLY, 0);
  ASSERT_EQ(LIBC_NAMESPACE::posix_spawn(&cpid, arg0, &file_actions, nullptr,
                                        argv, envp),
            0);
  ASSERT_TRUE(cpid > 0);
  int status;
  ASSERT_EQ(LIBC_NAMESPACE::waitpid(cpid, &status, 0), cpid);
  ASSERT_EQ(LIBC_NAMESPACE::posix_spawn_file_actions_destroy(&file_actions), 0);
  ASSERT_TRUE(WIFEXITED(status));
  int exit_status = WEXITSTATUS(status);
  ASSERT_EQ(exit_status, 0);
}

// A dup2 of a descriptor onto itself leaves it open in the child, which is how
// a caller hands one over that was opened close-on-exec.
void keep_a_descriptor_open(char **envp) {
  int fd = LIBC_NAMESPACE::open("testdata/posix_spawn.test", O_RDONLY);
  ASSERT_TRUE(fd > 0);
  ASSERT_EQ(LIBC_NAMESPACE::dup3(fd, CHILD_FD, O_CLOEXEC), CHILD_FD);
  ASSERT_EQ(LIBC_NAMESPACE::close(fd), 0);

  pid_t cpid;
  posix_spawn_file_actions_t file_actions;
  ASSERT_EQ(LIBC_NAMESPACE::posix_spawn_file_actions_init(&file_actions), 0);
  ASSERT_EQ(LIBC_NAMESPACE::posix_spawn_file_actions_adddup2(
                &file_actions, CHILD_FD, CHILD_FD),
            0);
  ASSERT_EQ(LIBC_NAMESPACE::posix_spawn(&cpid, arg0, &file_actions, nullptr,
                                        argv, envp),
            0);
  int status;
  ASSERT_EQ(LIBC_NAMESPACE::waitpid(cpid, &status, 0), cpid);
  ASSERT_EQ(LIBC_NAMESPACE::posix_spawn_file_actions_destroy(&file_actions), 0);
  ASSERT_EQ(LIBC_NAMESPACE::close(CHILD_FD), 0);
  ASSERT_TRUE(WIFEXITED(status));
  ASSERT_EQ(WEXITSTATUS(status), 0);
}

// With POSIX_SPAWN_CLOEXEC_DEFAULT a descriptor the parent left inheritable is
// closed in the child, and one that a file action opens still is not.
void close_the_inherited_descriptors(char **envp) {
  int fd = LIBC_NAMESPACE::open("testdata/posix_spawn.test", O_RDONLY);
  ASSERT_TRUE(fd > 0);
  ASSERT_EQ(LIBC_NAMESPACE::dup3(fd, CHILD_FD, 0), CHILD_FD);
  ASSERT_EQ(LIBC_NAMESPACE::close(fd), 0);

  posix_spawnattr_t attr;
  ASSERT_EQ(LIBC_NAMESPACE::posix_spawnattr_init(&attr), 0);
  ASSERT_EQ(LIBC_NAMESPACE::posix_spawnattr_setflags(
                &attr, POSIX_SPAWN_CLOEXEC_DEFAULT),
            0);

  pid_t cpid;
  int status;
  ASSERT_EQ(
      LIBC_NAMESPACE::posix_spawn(&cpid, arg0, nullptr, &attr, argv, envp), 0);
  ASSERT_EQ(LIBC_NAMESPACE::waitpid(cpid, &status, 0), cpid);
  ASSERT_TRUE(WIFEXITED(status));
  // The child could not read the descriptor.
  ASSERT_EQ(WEXITSTATUS(status), 1);

  posix_spawn_file_actions_t file_actions;
  ASSERT_EQ(LIBC_NAMESPACE::posix_spawn_file_actions_init(&file_actions), 0);
  ASSERT_EQ(
      LIBC_NAMESPACE::posix_spawn_file_actions_addopen(
          &file_actions, CHILD_FD, "testdata/posix_spawn.test", O_RDONLY, 0),
      0);
  ASSERT_EQ(LIBC_NAMESPACE::posix_spawn(&cpid, arg0, &file_actions, &attr, argv,
                                        envp),
            0);
  ASSERT_EQ(LIBC_NAMESPACE::waitpid(cpid, &status, 0), cpid);
  ASSERT_TRUE(WIFEXITED(status));
  ASSERT_EQ(WEXITSTATUS(status), 0);

  ASSERT_EQ(LIBC_NAMESPACE::posix_spawn_file_actions_destroy(&file_actions), 0);
  ASSERT_EQ(LIBC_NAMESPACE::posix_spawnattr_destroy(&attr), 0);
  ASSERT_EQ(LIBC_NAMESPACE::close(CHILD_FD), 0);
}

// A child that cannot be run at all is reported to whoever asked, rather than
// left to be found out from its exit status.
void report_a_child_that_cannot_run(char **envp) {
  char missing[] = "/nonexistent/not-a-program";
  char *missing_argv[] = {missing, nullptr};
  pid_t cpid = -1;
  ASSERT_EQ(LIBC_NAMESPACE::posix_spawn(&cpid, missing, nullptr, nullptr,
                                        missing_argv, envp),
            ENOENT);

  // A directory is there but is not something that can be run.
  char directory[] = "/";
  char *directory_argv[] = {directory, nullptr};
  ASSERT_EQ(LIBC_NAMESPACE::posix_spawn(&cpid, directory, nullptr, nullptr,
                                        directory_argv, envp),
            EACCES);
}

// posix_spawnp looks along PATH, and says the same when nothing there fits.
void search_the_path(char **envp) {
  char missing[] = "not-a-program-anywhere-on-the-path";
  char *missing_argv[] = {missing, nullptr};
  pid_t cpid = -1;
  ASSERT_EQ(LIBC_NAMESPACE::posix_spawnp(&cpid, missing, nullptr, nullptr,
                                         missing_argv, envp),
            ENOENT);

  // A name with a directory in it is used as given, with no search.
  char given[] = "./libc_posix_spawn_test_binary";
  char *given_argv[] = {given, nullptr};
  ASSERT_EQ(LIBC_NAMESPACE::posix_spawnp(&cpid, "/nonexistent/xyz", nullptr,
                                         nullptr, given_argv, envp),
            ENOENT);
}

TEST_MAIN([[maybe_unused]] int argc, [[maybe_unused]] char **argv,
          char **envp) {
  spawn_and_wait_for_normal_exit(envp);
  keep_a_descriptor_open(envp);
  close_the_inherited_descriptors(envp);
  report_a_child_that_cannot_run(envp);
  search_the_path(envp);
  return 0;
}
