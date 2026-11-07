//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the properties that are kept in a directory of areas, one for
/// each SELinux context, as Android's init makes them.
///
//===----------------------------------------------------------------------===//

#include "hdr/fcntl_macros.h"
#include "hdr/limits_macros.h"
#include "hdr/sys_mman_macros.h"
#include "hdr/system_properties_macros.h"
#include "hdr/types/mode_t.h"
#include "src/fcntl/open.h"
#include "src/string/strcmp.h"
#include "src/string/strlen.h"
#include "src/sys/mman/mmap.h"
#include "src/sys/mman/munmap.h"
#include "src/sys/stat/mkdir.h"
#include "src/sys/system_properties/__system_property_add.h"
#include "src/sys/system_properties/__system_property_area_init.h"
#include "src/sys/system_properties/__system_property_area_serial.h"
#include "src/sys/system_properties/__system_property_find.h"
#include "src/sys/system_properties/__system_property_find_nth.h"
#include "src/sys/system_properties/__system_property_foreach.h"
#include "src/sys/system_properties/__system_property_get.h"
#include "src/sys/system_properties/__system_property_set_filename.h"
#include "src/sys/system_properties/__system_property_update.h"
#include "src/sys/system_properties/property_area.h"
#include "src/sys/system_properties/property_info.h"
#include "src/unistd/close.h"
#include "src/unistd/rmdir.h"
#include "src/unistd/unlink.h"
#include "src/unistd/write.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"
#include "test/src/sys/system_properties/property_info_blob.h"

using LlvmLibcSystemPropertyDirectoryTest =
    LIBC_NAMESPACE::testing::ErrnoCheckingTest;

using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Succeeds;

namespace {

namespace sp = LIBC_NAMESPACE::system_properties;
namespace blob = property_info_blob;

int added(const char *name, const char *value) {
  return LIBC_NAMESPACE::__system_property_add(
      name, static_cast<unsigned>(LIBC_NAMESPACE::strlen(name)), value,
      static_cast<unsigned>(LIBC_NAMESPACE::strlen(value)));
}

void join(char *out, const char *directory, const char *leaf) {
  size_t n = 0;
  for (size_t i = 0; directory[i] != '\0'; ++i)
    out[n++] = directory[i];
  out[n++] = '/';
  for (size_t i = 0; leaf[i] != '\0'; ++i)
    out[n++] = leaf[i];
  out[n] = '\0';
}

bool write_info(const char *directory) {
  char path[PATH_MAX];
  join(path, directory, "property_info");
  unsigned char data[blob::SIZE];
  blob::fill(data);
  int fd = LIBC_NAMESPACE::open(path, O_WRONLY | O_CREAT, 0444);
  if (fd < 0)
    return false;
  const bool whole = LIBC_NAMESPACE::write(fd, data, sizeof(data)) ==
                     static_cast<ssize_t>(sizeof(data));
  LIBC_NAMESPACE::close(fd);
  return whole;
}

// The area in a file of the directory, mapped to look at what is in it.
sp::Area *map_context(const char *directory, const char *context) {
  char path[PATH_MAX];
  join(path, directory, context);
  int fd = LIBC_NAMESPACE::open(path, O_RDONLY);
  if (fd < 0)
    return nullptr;
  void *memory = LIBC_NAMESPACE::mmap(nullptr, sp::AREA_SIZE, PROT_READ,
                                      MAP_SHARED, fd, 0);
  LIBC_NAMESPACE::close(fd);
  return memory == MAP_FAILED ? nullptr : static_cast<sp::Area *>(memory);
}

int properties_counted;
void count(const prop_info *, void *cookie) {
  ++*static_cast<int *>(cookie);
  ++properties_counted;
}

} // anonymous namespace

TEST_F(LlvmLibcSystemPropertyDirectoryTest, ContextOfAProperty) {
  alignas(uint32_t) unsigned char data[blob::SIZE];
  blob::fill(data);
  sp::PropertyInfo info(data, sizeof(data));
  ASSERT_TRUE(info.valid());
  ASSERT_EQ(info.num_contexts(), 2u);
  EXPECT_EQ(LIBC_NAMESPACE::strcmp(info.context(0), blob::DEFAULT_CONTEXT), 0);
  EXPECT_EQ(LIBC_NAMESPACE::strcmp(info.context(1), blob::BUILD_CONTEXT), 0);
  EXPECT_TRUE(info.context(2) == nullptr);

  // A prefix that ends at a dot, with the rest of the name after it.
  EXPECT_EQ(info.context_of("ro.build.id"), 1u);
  EXPECT_EQ(info.context_of("ro.build.version.sdk"), 1u);
  // Not under that prefix.
  EXPECT_EQ(info.context_of("ro.build"), 0u);
  EXPECT_EQ(info.context_of("ro.product.model"), 0u);
  EXPECT_EQ(info.context_of("sys.state"), 0u);
  // An exact match is only that name.
  EXPECT_EQ(info.context_of("debug"), 1u);
  EXPECT_EQ(info.context_of("debugger"), 0u);
  EXPECT_EQ(info.context_of("debug.x"), 0u);
}

TEST_F(LlvmLibcSystemPropertyDirectoryTest, InfoThatCannotBeTrusted) {
  alignas(uint32_t) unsigned char data[blob::SIZE];
  blob::fill(data);
  // Cut short, so that what it points to is not all there.
  EXPECT_FALSE(sp::PropertyInfo(data, 100).valid());
  EXPECT_FALSE(sp::PropertyInfo(data, 8).valid());
  EXPECT_FALSE(sp::PropertyInfo().valid());

  // From a newer layout than this one understands.
  reinterpret_cast<uint32_t *>(data)[1] = 2;
  EXPECT_FALSE(sp::PropertyInfo(data, sizeof(data)).valid());
  blob::fill(data);

  // A root that is not in the area.
  reinterpret_cast<uint32_t *>(data)[5] = 100000;
  EXPECT_FALSE(sp::PropertyInfo(data, sizeof(data)).valid());
  blob::fill(data);

  // A name that never ends is not a context. The search for it is left alone.
  reinterpret_cast<uint32_t *>(data)[7] = blob::SIZE - 2;
  data[blob::SIZE - 2] = 'x';
  data[blob::SIZE - 1] = 'y';
  sp::PropertyInfo unterminated(data, sizeof(data));
  EXPECT_TRUE(unterminated.context(0) == nullptr);
}

// One test with the area, as it is process wide and a second one could not make
// it again.
TEST_F(LlvmLibcSystemPropertyDirectoryTest, AreaForEachContext) {
  const char *directory = libc_make_test_file_path("properties_directory");
  ASSERT_THAT(LIBC_NAMESPACE::mkdir(directory, 0755), Succeeds(0));
  ASSERT_TRUE(write_info(directory));
  ASSERT_EQ(LIBC_NAMESPACE::__system_property_set_filename(directory), 0);
  ASSERT_EQ(LIBC_NAMESPACE::__system_property_area_init(), 0);

  // An area for each context, and the one with the serial.
  char path[PATH_MAX];
  const char *areas[] = {"properties_serial", blob::DEFAULT_CONTEXT,
                         blob::BUILD_CONTEXT};
  for (const char *leaf : areas) {
    join(path, directory, leaf);
    int fd = LIBC_NAMESPACE::open(path, O_RDONLY);
    EXPECT_GE(fd, 0);
    if (fd >= 0)
      LIBC_NAMESPACE::close(fd);
  }

  const uint32_t serial_before =
      LIBC_NAMESPACE::__system_property_area_serial();
  ASSERT_EQ(added("ro.build.id", "TQ3A"), 0);
  ASSERT_EQ(added("ro.product.model", "Axion"), 0);
  ASSERT_EQ(added("debug", "1"), 0);
  EXPECT_NE(LIBC_NAMESPACE::__system_property_area_serial(), serial_before);
  // Twice is once too many, whichever area it would be in.
  EXPECT_EQ(added("ro.build.id", "other"), -1);
  ASSERT_ERRNO_EQ(EEXIST);

  char value[PROP_VALUE_MAX];
  EXPECT_EQ(LIBC_NAMESPACE::__system_property_get("ro.build.id", value), 4);
  EXPECT_EQ(LIBC_NAMESPACE::strcmp(value, "TQ3A"), 0);
  EXPECT_EQ(LIBC_NAMESPACE::__system_property_get("ro.product.model", value),
            5);
  EXPECT_EQ(LIBC_NAMESPACE::strcmp(value, "Axion"), 0);
  EXPECT_TRUE(LIBC_NAMESPACE::__system_property_find("ro.build") == nullptr);

  // Each is in the area of its context and not in the other.
  sp::Area *build = map_context(directory, blob::BUILD_CONTEXT);
  sp::Area *other = map_context(directory, blob::DEFAULT_CONTEXT);
  ASSERT_TRUE(build != nullptr);
  ASSERT_TRUE(other != nullptr);
  EXPECT_TRUE(build->find("ro.build.id") != nullptr);
  EXPECT_TRUE(build->find("debug") != nullptr);
  EXPECT_TRUE(build->find("ro.product.model") == nullptr);
  EXPECT_TRUE(other->find("ro.product.model") != nullptr);
  EXPECT_TRUE(other->find("ro.build.id") == nullptr);

  // Updating changes it where it is, and the serial moves.
  const prop_info *id = LIBC_NAMESPACE::__system_property_find("ro.build.id");
  ASSERT_TRUE(id != nullptr);
  const uint32_t serial_updated =
      LIBC_NAMESPACE::__system_property_area_serial();
  ASSERT_EQ(LIBC_NAMESPACE::__system_property_update(
                const_cast<prop_info *>(id), "TQ3B", 4),
            0);
  EXPECT_NE(LIBC_NAMESPACE::__system_property_area_serial(), serial_updated);
  EXPECT_EQ(LIBC_NAMESPACE::__system_property_get("ro.build.id", value), 4);
  EXPECT_EQ(LIBC_NAMESPACE::strcmp(value, "TQ3B"), 0);

  // Every property of every area is walked, and found by its place in that.
  int seen = 0;
  properties_counted = 0;
  EXPECT_EQ(LIBC_NAMESPACE::__system_property_foreach(count, &seen), 0);
  EXPECT_EQ(seen, 3);
  EXPECT_TRUE(LIBC_NAMESPACE::__system_property_find_nth(2) != nullptr);
  EXPECT_TRUE(LIBC_NAMESPACE::__system_property_find_nth(3) == nullptr);

  LIBC_NAMESPACE::munmap(build, sp::AREA_SIZE);
  LIBC_NAMESPACE::munmap(other, sp::AREA_SIZE);
  const char *files[] = {"properties_serial", "property_info",
                         blob::DEFAULT_CONTEXT, blob::BUILD_CONTEXT};
  for (const char *leaf : files) {
    join(path, directory, leaf);
    LIBC_NAMESPACE::unlink(path);
  }
  LIBC_NAMESPACE::rmdir(directory);
}
