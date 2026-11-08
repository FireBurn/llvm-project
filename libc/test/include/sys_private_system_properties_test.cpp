//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
///
/// \file
/// Tests that sys/_system_properties.h brings in the property interface.
///
//===----------------------------------------------------------------------===//

#include "include/sys/_system_properties.h"
#include "test/UnitTest/Test.h"

#ifndef PROP_VALUE_MAX
#error "<sys/_system_properties.h> does not bring in <sys/system_properties.h>"
#endif

TEST(LlvmLibcSysSystemPropertiesTest, CarriesTheInterface) {
  EXPECT_EQ(static_cast<int>(PROP_VALUE_MAX), 92);
  EXPECT_TRUE(&__system_property_find != nullptr);
  EXPECT_TRUE(&__system_property_serial != nullptr);
}
