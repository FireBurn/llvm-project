//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the large file support names the public headers declare.
///
//===----------------------------------------------------------------------===//

#define _LARGEFILE64_SOURCE 1

#include "include/fcntl.h"
#include "include/sys/stat.h"
#include "include/sys/types.h"
#include "include/unistd.h"
#include "test/UnitTest/Test.h"

// A file offset is sixty four bits wide here to begin with, so each of these
// names stands for the ordinary interface.
TEST(LlvmLibcLfs64Test, TypesAreTheOrdinaryOnes) {
  EXPECT_EQ(sizeof(off64_t), sizeof(off_t));
  EXPECT_EQ(sizeof(off64_t), static_cast<size_t>(8));
  EXPECT_EQ(sizeof(ino64_t), sizeof(ino_t));
  EXPECT_EQ(sizeof(blkcnt64_t), sizeof(blkcnt_t));
  EXPECT_EQ(sizeof(struct stat64), sizeof(struct stat));
}

// A program may take the address of one of these, which sqlite does to build
// the table of the calls it makes, so they have to be functions and not only
// macros.
TEST(LlvmLibcLfs64Test, CallsAreDeclaredAsFunctions) {
  off64_t (*seek)(int, off64_t, int) = lseek64;
  ssize_t (*read_at)(int, void *, size_t, off64_t) = pread64;
  ssize_t (*write_at)(int, const void *, size_t, off64_t) = pwrite64;
  int (*truncate_to)(const char *, off64_t) = truncate64;
  int (*open_file)(const char *, int, ...) = open64;
  EXPECT_TRUE(seek != nullptr);
  EXPECT_TRUE(read_at != nullptr);
  EXPECT_TRUE(write_at != nullptr);
  EXPECT_TRUE(truncate_to != nullptr);
  EXPECT_TRUE(open_file != nullptr);
}

// The names of records are macros, since each stands for a type as well.
TEST(LlvmLibcLfs64Test, RecordNamesAreDefined) {
#ifndef stat64
  FAIL() << "stat64 is not defined";
#endif
}

// Only dirent.h declares DIR, so a program that reads fcntl.h may use the name
// for something of its own, as libpcap's grammar does for a token.
namespace {
enum { DIR = 7 };
} // namespace

TEST(LlvmLibcLfs64Test, DirIsNotDeclaredByFcntl) {
  EXPECT_EQ(static_cast<int>(DIR), 7);
}
