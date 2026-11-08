//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Integration test that the C runtime objects are plain ELF.
///
//===----------------------------------------------------------------------===//

#include "hdr/fcntl_macros.h"
#include "src/fcntl/open.h"
#include "src/unistd/close.h"
#include "src/unistd/read.h"
#include "test/IntegrationTest/test.h"

// A build with LTO turns an object into bitcode, which lld then merges into
// the LTO unit of whatever links it. Programs that split their unit differently
// fail to link, so the start files must stay ELF.
TEST_MAIN(int argc, char **argv, char **) {
  ASSERT_TRUE(argc > 1);
  for (int i = 1; i < argc; ++i) {
    int fd = LIBC_NAMESPACE::open(argv[i], O_RDONLY);
    ASSERT_TRUE(fd >= 0);
    unsigned char magic[4] = {};
    ASSERT_TRUE(LIBC_NAMESPACE::read(fd, magic, sizeof(magic)) == 4);
    ASSERT_EQ(LIBC_NAMESPACE::close(fd), 0);
    ASSERT_TRUE(magic[0] == 0x7f && magic[1] == 'E' && magic[2] == 'L' &&
                magic[3] == 'F');
  }
  return 0;
}
