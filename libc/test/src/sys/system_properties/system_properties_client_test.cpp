//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for reading an area of system properties the way a client does.
///
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/fcntl_macros.h"
#include "hdr/sys_mman_macros.h"
#include "hdr/sys_stat_macros.h"
#include "hdr/types/mode_t.h"
#include "hdr/system_properties_macros.h"
#include "src/__support/libc_errno.h"
#include "src/fcntl/open.h"
#include "src/string/strcmp.h"
#include "src/sys/mman/mmap.h"
#include "src/sys/mman/munmap.h"
#include "src/sys/system_properties/__system_property_find.h"
#include "src/sys/system_properties/__system_property_get.h"
#include "src/sys/system_properties/__system_property_set_filename.h"
#include "src/sys/system_properties/__system_property_update.h"
#include "src/sys/system_properties/property_area.h"
#include "src/unistd/close.h"
#include "src/unistd/ftruncate.h"
#include "src/unistd/unlink.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/Test.h"

using LlvmLibcSystemPropertiesClientTest =
    LIBC_NAMESPACE::testing::ErrnoCheckingTest;

namespace {

namespace sp = LIBC_NAMESPACE::system_properties;

// Writes an area the way the process that serves the properties would.
bool write_area(const char *path, mode_t mode, size_t size) {
  // Created read only, which still lets the descriptor it returns write.
  int fd = LIBC_NAMESPACE::open(path, O_RDWR | O_CREAT, mode);
  if (fd < 0)
    return false;
  bool written = false;
  if (LIBC_NAMESPACE::ftruncate(fd, static_cast<off_t>(size)) == 0) {
    void *memory = LIBC_NAMESPACE::mmap(nullptr, sp::AREA_SIZE,
                                        PROT_READ | PROT_WRITE, MAP_SHARED, fd,
                                        0);
    if (memory != MAP_FAILED) {
      sp::Area *area = static_cast<sp::Area *>(memory);
      area->init();
      written = area->add("ro.product.model", 16, "Axion", 5) &&
                area->add("sys.state", 9, "up", 2);
      LIBC_NAMESPACE::munmap(memory, sp::AREA_SIZE);
    }
  }
  LIBC_NAMESPACE::close(fd);
  return written;
}

} // anonymous namespace

TEST_F(LlvmLibcSystemPropertiesClientTest, ReadsWhatAServerWrote) {
  const char *path = libc_make_test_file_path("properties_client");
  ASSERT_TRUE(write_area(path, 0444, sp::AREA_SIZE));
  ASSERT_EQ(LIBC_NAMESPACE::__system_property_set_filename(path), 0);

  char value[PROP_VALUE_MAX];
  EXPECT_EQ(LIBC_NAMESPACE::__system_property_get("ro.product.model", value),
            5);
  EXPECT_EQ(LIBC_NAMESPACE::strcmp(value, "Axion"), 0);
  const prop_info *state = LIBC_NAMESPACE::__system_property_find("sys.state");
  ASSERT_TRUE(state != nullptr);
  EXPECT_TRUE(LIBC_NAMESPACE::__system_property_find("sys.missing") == nullptr);

  // A client only reads.
  EXPECT_EQ(LIBC_NAMESPACE::__system_property_update(
                const_cast<prop_info *>(state), "down", 4),
            -1);
  ASSERT_ERRNO_EQ(EPERM);
  libc_errno = 0;
  LIBC_NAMESPACE::unlink(path);
}
