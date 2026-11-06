//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for refusing an area of system properties that others can write.
///
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/fcntl_macros.h"
#include "hdr/sys_mman_macros.h"
#include "hdr/sys_stat_macros.h"
#include "src/__support/libc_errno.h"
#include "src/fcntl/open.h"
#include "src/sys/mman/mmap.h"
#include "src/sys/mman/munmap.h"
#include "src/sys/stat/chmod.h"
#include "src/sys/system_properties/__system_property_find.h"
#include "src/sys/system_properties/__system_property_set_filename.h"
#include "src/sys/system_properties/__system_properties_init.h"
#include "src/sys/system_properties/property_area.h"
#include "src/unistd/close.h"
#include "src/unistd/ftruncate.h"
#include "src/unistd/unlink.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/Test.h"

using LlvmLibcSystemPropertiesUntrustedTest =
    LIBC_NAMESPACE::testing::ErrnoCheckingTest;

namespace {

namespace sp = LIBC_NAMESPACE::system_properties;

bool write_area(const char *path, mode_t mode, size_t size) {
  int fd = LIBC_NAMESPACE::open(path, O_RDWR | O_CREAT, 0600);
  if (fd < 0)
    return false;
  bool written = false;
  if (LIBC_NAMESPACE::ftruncate(fd, static_cast<off_t>(size)) == 0) {
    void *memory = LIBC_NAMESPACE::mmap(nullptr, sp::AREA_SIZE,
                                        PROT_READ | PROT_WRITE, MAP_SHARED, fd,
                                        0);
    if (memory != MAP_FAILED) {
      static_cast<sp::Area *>(memory)->init();
      written = true;
      LIBC_NAMESPACE::munmap(memory, sp::AREA_SIZE);
    }
  }
  LIBC_NAMESPACE::close(fd);
  return written && LIBC_NAMESPACE::chmod(path, mode) == 0;
}

} // anonymous namespace

TEST_F(LlvmLibcSystemPropertiesUntrustedTest, WritableByOthersIsNotUsed) {
  const char *path = libc_make_test_file_path("properties_untrusted");
  ASSERT_TRUE(write_area(path, 0666, sp::AREA_SIZE));
  ASSERT_EQ(LIBC_NAMESPACE::__system_property_set_filename(path), 0);
  EXPECT_EQ(LIBC_NAMESPACE::__system_properties_init(), -1);
  ASSERT_ERRNO_EQ(EPERM);
  libc_errno = 0;
  LIBC_NAMESPACE::unlink(path);
}

TEST_F(LlvmLibcSystemPropertiesUntrustedTest, TooSmallIsNotUsed) {
  const char *path = libc_make_test_file_path("properties_small");
  ASSERT_TRUE(write_area(path, 0444, 4096));
  ASSERT_EQ(LIBC_NAMESPACE::__system_property_set_filename(path), 0);
  EXPECT_TRUE(LIBC_NAMESPACE::__system_property_find("ro.any") == nullptr);
  ASSERT_ERRNO_EQ(EPERM);
  libc_errno = 0;
  LIBC_NAMESPACE::unlink(path);
}
