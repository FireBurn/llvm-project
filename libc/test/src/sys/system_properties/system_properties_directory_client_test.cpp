//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for reading the properties from a directory of areas the way a
/// program does that is not the one that serves them.
///
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/fcntl_macros.h"
#include "hdr/limits_macros.h"
#include "hdr/sys_mman_macros.h"
#include "hdr/system_properties_macros.h"
#include "src/fcntl/open.h"
#include "src/string/strcmp.h"
#include "src/string/strlen.h"
#include "src/sys/mman/mmap.h"
#include "src/sys/mman/munmap.h"
#include "src/sys/stat/mkdir.h"
#include "src/sys/system_properties/__system_properties_init.h"
#include "src/sys/system_properties/__system_property_find.h"
#include "src/sys/system_properties/__system_property_foreach.h"
#include "src/sys/system_properties/__system_property_get.h"
#include "src/sys/system_properties/__system_property_set_filename.h"
#include "src/sys/system_properties/property_area.h"
#include "src/unistd/close.h"
#include "src/unistd/ftruncate.h"
#include "src/unistd/rmdir.h"
#include "src/unistd/unlink.h"
#include "src/unistd/write.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"
#include "test/src/sys/system_properties/property_info_blob.h"

using LlvmLibcSystemPropertyDirectoryClientTest =
    LIBC_NAMESPACE::testing::ErrnoCheckingTest;

using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Succeeds;

namespace {

namespace sp = LIBC_NAMESPACE::system_properties;
namespace blob = property_info_blob;

void join(char *out, const char *directory, const char *leaf) {
  size_t n = 0;
  for (size_t i = 0; directory[i] != '\0'; ++i)
    out[n++] = directory[i];
  out[n++] = '/';
  for (size_t i = 0; leaf[i] != '\0'; ++i)
    out[n++] = leaf[i];
  out[n] = '\0';
}

// Writes an area the way the process that serves the properties would.
bool write_area(const char *directory, const char *leaf, const char *name,
                const char *value) {
  char path[PATH_MAX];
  join(path, directory, leaf);
  int fd = LIBC_NAMESPACE::open(path, O_RDWR | O_CREAT, 0444);
  if (fd < 0)
    return false;
  bool written = false;
  if (LIBC_NAMESPACE::ftruncate(fd, static_cast<off_t>(sp::AREA_SIZE)) == 0) {
    void *memory = LIBC_NAMESPACE::mmap(
        nullptr, sp::AREA_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (memory != MAP_FAILED) {
      sp::Area *area = static_cast<sp::Area *>(memory);
      area->init();
      written =
          name == nullptr || area->add(name, LIBC_NAMESPACE::strlen(name),
                                       value, LIBC_NAMESPACE::strlen(value));
      LIBC_NAMESPACE::munmap(memory, sp::AREA_SIZE);
    }
  }
  LIBC_NAMESPACE::close(fd);
  return written;
}

bool write_info(const char *directory) {
  char path[PATH_MAX];
  join(path, directory, "property_info");
  alignas(uint32_t) unsigned char data[blob::SIZE];
  blob::fill(data);
  int fd = LIBC_NAMESPACE::open(path, O_WRONLY | O_CREAT, 0444);
  if (fd < 0)
    return false;
  const bool whole = LIBC_NAMESPACE::write(fd, data, sizeof(data)) ==
                     static_cast<ssize_t>(sizeof(data));
  LIBC_NAMESPACE::close(fd);
  return whole;
}

int walked;
void walk(const prop_info *, void *) { ++walked; }

} // anonymous namespace

// One test, as what is mapped is process wide.
TEST_F(LlvmLibcSystemPropertyDirectoryClientTest, ReadsEachContext) {
  const char *directory =
      libc_make_test_file_path("properties_directory_client");
  ASSERT_THAT(LIBC_NAMESPACE::mkdir(directory, 0755), Succeeds(0));
  ASSERT_TRUE(write_info(directory));
  ASSERT_TRUE(write_area(directory, "properties_serial", nullptr, nullptr));
  ASSERT_TRUE(
      write_area(directory, blob::BUILD_CONTEXT, "ro.build.id", "TQ3A"));
  // The area of the default context is not there yet: what is in the others is
  // still read.
  ASSERT_EQ(LIBC_NAMESPACE::__system_property_set_filename(directory), 0);
  ASSERT_EQ(LIBC_NAMESPACE::__system_properties_init(), 0);

  char value[PROP_VALUE_MAX];
  EXPECT_EQ(LIBC_NAMESPACE::__system_property_get("ro.build.id", value), 4);
  EXPECT_EQ(LIBC_NAMESPACE::strcmp(value, "TQ3A"), 0);
  // A property of the context with no area is not found, and nothing else is
  // the worse for it.
  EXPECT_TRUE(LIBC_NAMESPACE::__system_property_find("ro.product.model") ==
              nullptr);
  ASSERT_ERRNO_EQ(ENOENT);
  EXPECT_EQ(LIBC_NAMESPACE::__system_property_get("ro.product.model", value),
            0);
  ASSERT_ERRNO_EQ(ENOENT);
  EXPECT_EQ(value[0], '\0');

  walked = 0;
  EXPECT_EQ(LIBC_NAMESPACE::__system_property_foreach(walk, nullptr), 0);
  EXPECT_EQ(walked, 1);

  char path[PATH_MAX];
  const char *files[] = {"properties_serial", "property_info",
                         blob::BUILD_CONTEXT};
  for (const char *leaf : files) {
    join(path, directory, leaf);
    LIBC_NAMESPACE::unlink(path);
  }
  LIBC_NAMESPACE::rmdir(directory);
}
