//===-- Unittests for vmsplice and tee ------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/types/struct_iovec.h"
#include "src/fcntl/tee.h"
#include "src/fcntl/vmsplice.h"
#include "src/unistd/close.h"
#include "src/unistd/pipe.h"
#include "src/unistd/read.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/Test.h"

using LlvmLibcVmspliceTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;

// What the caller's own memory holds goes into the pipe without a copy, and
// reads back out of it unchanged.
TEST_F(LlvmLibcVmspliceTest, WhatWentInComesBackOut) {
  int ends[2];
  ASSERT_EQ(LIBC_NAMESPACE::pipe(ends), 0);

  char text[] = "hello";
  struct iovec piece = {text, sizeof(text) - 1};
  ASSERT_EQ(LIBC_NAMESPACE::vmsplice(ends[1], &piece, 1, 0),
            ssize_t(sizeof(text) - 1));

  char back[16] = {};
  ASSERT_EQ(LIBC_NAMESPACE::read(ends[0], back, sizeof(back)),
            ssize_t(sizeof(text) - 1));
  ASSERT_EQ(back[0], 'h');
  ASSERT_EQ(back[4], 'o');

  ASSERT_EQ(LIBC_NAMESPACE::close(ends[0]), 0);
  ASSERT_EQ(LIBC_NAMESPACE::close(ends[1]), 0);
}

// tee copies from one pipe to another without taking anything out of the
// first, so the same bytes are read twice.
TEST_F(LlvmLibcVmspliceTest, TeeLeavesTheBytesWhereTheyWere) {
  int first[2];
  int second[2];
  ASSERT_EQ(LIBC_NAMESPACE::pipe(first), 0);
  ASSERT_EQ(LIBC_NAMESPACE::pipe(second), 0);

  char text[] = "twice";
  struct iovec piece = {text, sizeof(text) - 1};
  ASSERT_EQ(LIBC_NAMESPACE::vmsplice(first[1], &piece, 1, 0),
            ssize_t(sizeof(text) - 1));

  ASSERT_EQ(LIBC_NAMESPACE::tee(first[0], second[1], 64, 0),
            ssize_t(sizeof(text) - 1));

  char from_second[16] = {};
  ASSERT_EQ(LIBC_NAMESPACE::read(second[0], from_second, sizeof(from_second)),
            ssize_t(sizeof(text) - 1));

  char from_first[16] = {};
  ASSERT_EQ(LIBC_NAMESPACE::read(first[0], from_first, sizeof(from_first)),
            ssize_t(sizeof(text) - 1));

  for (size_t i = 0; i < sizeof(text) - 1; ++i)
    ASSERT_EQ(from_first[i], from_second[i]);

  ASSERT_EQ(LIBC_NAMESPACE::close(first[0]), 0);
  ASSERT_EQ(LIBC_NAMESPACE::close(first[1]), 0);
  ASSERT_EQ(LIBC_NAMESPACE::close(second[0]), 0);
  ASSERT_EQ(LIBC_NAMESPACE::close(second[1]), 0);
}
