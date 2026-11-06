//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the system property functions.
///
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/system_properties_macros.h"
#include "hdr/time_macros.h"
#include "hdr/types/struct_timespec.h"
#include "src/__support/libc_errno.h"
#include "src/string/strcmp.h"
#include "src/string/strlen.h"
#include "src/sys/system_properties/__system_properties_init.h"
#include "src/sys/system_properties/__system_property_add.h"
#include "src/sys/system_properties/__system_property_area_init.h"
#include "src/sys/system_properties/__system_property_area_serial.h"
#include "src/sys/system_properties/__system_property_find.h"
#include "src/sys/system_properties/__system_property_find_nth.h"
#include "src/sys/system_properties/__system_property_foreach.h"
#include "src/sys/system_properties/__system_property_get.h"
#include "src/sys/system_properties/__system_property_read.h"
#include "src/sys/system_properties/__system_property_read_callback.h"
#include "src/sys/system_properties/__system_property_serial.h"
#include "src/sys/system_properties/__system_property_set_filename.h"
#include "src/sys/system_properties/__system_property_update.h"
#include "src/sys/system_properties/__system_property_wait.h"
#include "src/sys/system_properties/__system_property_wait_any.h"
#include "src/unistd/unlink.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/Test.h"

#include <stdint.h>

using LlvmLibcSystemPropertiesTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;

namespace {

int added(const char *name, const char *value) {
  return LIBC_NAMESPACE::__system_property_add(
      name, static_cast<unsigned>(LIBC_NAMESPACE::strlen(name)), value,
      static_cast<unsigned>(LIBC_NAMESPACE::strlen(value)));
}

int properties_counted;
void count(const prop_info *, void *cookie) {
  ++*static_cast<int *>(cookie);
  ++properties_counted;
}

struct Seen {
  char name[64];
  char value[256];
  uint32_t serial;
};
void remember(void *cookie, const char *name, const char *value,
              uint32_t serial) {
  Seen *seen = static_cast<Seen *>(cookie);
  size_t i = 0;
  for (; name[i] != '\0' && i < 63; ++i)
    seen->name[i] = name[i];
  seen->name[i] = '\0';
  for (i = 0; value[i] != '\0' && i < 255; ++i)
    seen->value[i] = value[i];
  seen->value[i] = '\0';
  seen->serial = serial;
}

} // anonymous namespace

// One test, as the area is process wide and a second one could not make it
// again.
TEST_F(LlvmLibcSystemPropertiesTest, AreaOfProperties) {
  const char *path = libc_make_test_file_path("properties_area");
  ASSERT_EQ(LIBC_NAMESPACE::__system_property_set_filename(path), 0);
  ASSERT_EQ(LIBC_NAMESPACE::__system_property_area_init(), 0);

  EXPECT_TRUE(LIBC_NAMESPACE::__system_property_find("ro.build.id") == nullptr);
  char value[PROP_VALUE_MAX];
  value[0] = 'x';
  EXPECT_EQ(LIBC_NAMESPACE::__system_property_get("ro.build.id", value), 0);
  EXPECT_EQ(value[0], '\0');

  const uint32_t area_before = LIBC_NAMESPACE::__system_property_area_serial();
  ASSERT_EQ(added("ro.build.id", "TQ3A"), 0);
  ASSERT_EQ(added("ro.build.version.sdk", "34"), 0);
  ASSERT_EQ(added("sys.boot_completed", "0"), 0);
  ASSERT_EQ(added("persist.sys.locale", "en-GB"), 0);
  EXPECT_NE(LIBC_NAMESPACE::__system_property_area_serial(), area_before);

  EXPECT_EQ(LIBC_NAMESPACE::__system_property_get("ro.build.id", value), 4);
  EXPECT_EQ(LIBC_NAMESPACE::strcmp(value, "TQ3A"), 0);
  EXPECT_EQ(LIBC_NAMESPACE::__system_property_get("ro.build.version.sdk", value),
            2);
  EXPECT_EQ(LIBC_NAMESPACE::strcmp(value, "34"), 0);

  // A name that is a prefix of another is a property of its own only if it
  // was added.
  EXPECT_TRUE(LIBC_NAMESPACE::__system_property_find("ro.build") == nullptr);
  EXPECT_TRUE(LIBC_NAMESPACE::__system_property_find("ro.build.version") ==
              nullptr);

  const prop_info *id = LIBC_NAMESPACE::__system_property_find("ro.build.id");
  ASSERT_TRUE(id != nullptr);
  char name[PROP_NAME_MAX + 64];
  EXPECT_EQ(LIBC_NAMESPACE::__system_property_read(id, name, value), 4);
  EXPECT_EQ(LIBC_NAMESPACE::strcmp(name, "ro.build.id"), 0);
  EXPECT_EQ(LIBC_NAMESPACE::strcmp(value, "TQ3A"), 0);

  Seen seen = {};
  LIBC_NAMESPACE::__system_property_read_callback(id, remember, &seen);
  EXPECT_EQ(LIBC_NAMESPACE::strcmp(seen.name, "ro.build.id"), 0);
  EXPECT_EQ(LIBC_NAMESPACE::strcmp(seen.value, "TQ3A"), 0);
  EXPECT_EQ(seen.serial >> 24, 4u);

  // Adding what is there already is refused, and says so.
  EXPECT_EQ(added("ro.build.id", "other"), -1);
  ASSERT_ERRNO_EQ(EEXIST);
  libc_errno = 0;
  EXPECT_EQ(added("", "empty"), -1);
  ASSERT_ERRNO_EQ(EINVAL);
  libc_errno = 0;
  // A value too long for a property that can change.
  char long_value[200];
  for (size_t i = 0; i < sizeof(long_value) - 1; ++i)
    long_value[i] = 'v';
  long_value[sizeof(long_value) - 1] = '\0';
  EXPECT_EQ(added("sys.too_long", long_value), -1);
  ASSERT_ERRNO_EQ(EINVAL);
  libc_errno = 0;

  int walked = 0;
  properties_counted = 0;
  EXPECT_EQ(LIBC_NAMESPACE::__system_property_foreach(count, &walked), 0);
  EXPECT_EQ(walked, 4);
  EXPECT_TRUE(LIBC_NAMESPACE::__system_property_find_nth(3) != nullptr);
  EXPECT_TRUE(LIBC_NAMESPACE::__system_property_find_nth(4) == nullptr);

  // A read only property may have a value of any length, which only the
  // callback can read in full.
  ASSERT_EQ(added("ro.long", long_value), 0);
  const prop_info *longer = LIBC_NAMESPACE::__system_property_find("ro.long");
  ASSERT_TRUE(longer != nullptr);
  LIBC_NAMESPACE::__system_property_read_callback(longer, remember, &seen);
  EXPECT_EQ(LIBC_NAMESPACE::strlen(seen.value), size_t(199));
  EXPECT_EQ(LIBC_NAMESPACE::__system_property_read(longer, nullptr, value),
            int(LIBC_NAMESPACE::strlen(
                "Must use __system_property_read_callback() to read")));

  // A change moves the serial of the property and the area on.
  const prop_info *boot =
      LIBC_NAMESPACE::__system_property_find("sys.boot_completed");
  ASSERT_TRUE(boot != nullptr);
  const uint32_t serial_before = LIBC_NAMESPACE::__system_property_serial(boot);
  const uint32_t area_serial = LIBC_NAMESPACE::__system_property_area_serial();
  ASSERT_EQ(LIBC_NAMESPACE::__system_property_update(
                const_cast<prop_info *>(boot), "1", 1),
            0);
  EXPECT_NE(LIBC_NAMESPACE::__system_property_serial(boot), serial_before);
  EXPECT_NE(LIBC_NAMESPACE::__system_property_area_serial(), area_serial);
  EXPECT_EQ(LIBC_NAMESPACE::__system_property_get("sys.boot_completed", value),
            1);
  EXPECT_EQ(LIBC_NAMESPACE::strcmp(value, "1"), 0);
  EXPECT_EQ(LIBC_NAMESPACE::__system_property_update(
                const_cast<prop_info *>(boot), long_value, 199),
            -1);
  ASSERT_ERRNO_EQ(EINVAL);
  libc_errno = 0;

  // Waiting for something that has already happened returns at once. For
  // what has not, it gives up after the time asked for.
  uint32_t newer = 0;
  EXPECT_TRUE(LIBC_NAMESPACE::__system_property_wait(boot, serial_before,
                                                      &newer, nullptr));
  EXPECT_NE(newer, serial_before);
  const struct timespec brief = {0, 20 * 1000 * 1000};
  EXPECT_FALSE(LIBC_NAMESPACE::__system_property_wait(boot, newer, &newer,
                                                       &brief));
  EXPECT_NE(LIBC_NAMESPACE::__system_property_wait_any(area_serial),
            area_serial);

  EXPECT_EQ(LIBC_NAMESPACE::__system_properties_init(), 0);
  LIBC_NAMESPACE::unlink(path);
}

TEST(LlvmLibcSystemPropertiesTest, ServiceProtocolConstants) {
  EXPECT_EQ(PROP_MSG_SETPROP2, 0x00020001);
  EXPECT_EQ(PROP_ERROR_READ_ONLY_PROPERTY, 0x000B);
  EXPECT_EQ(PROP_ERROR_INVALID_NAME, 0x0010);
  EXPECT_EQ(PROP_ERROR_INVALID_VALUE, 0x0014);
  EXPECT_EQ(PROP_ERROR_SET_FAILED, 0x0024);
  EXPECT_STREQ(PROP_DIRNAME, "/dev/__properties__");
}
