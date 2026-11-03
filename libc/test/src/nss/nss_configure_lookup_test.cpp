//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the tests of __nss_configure_lookup.
///
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "src/__support/nss/config.h"
#include "src/nss/__nss_configure_lookup.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/Test.h"

using LlvmLibcNssConfigureLookupTest =
    LIBC_NAMESPACE::testing::ErrnoCheckingTest;

TEST_F(LlvmLibcNssConfigureLookupTest, ReplacesTheServicesOfADatabase) {
  LIBC_NAMESPACE::nss::TESTONLY_set_config_path("/nonexistent/nsswitch.conf");
  ASSERT_EQ(LIBC_NAMESPACE::__nss_configure_lookup("hosts", "dns files"), 0);
  const auto *source = LIBC_NAMESPACE::nss::sources(LIBC_NAMESPACE::nss::HOSTS);
  ASSERT_STREQ(source->name, "dns");
  ASSERT_STREQ(source->next->name, "files");
  LIBC_NAMESPACE::nss::TESTONLY_reset_config_path();
}

TEST_F(LlvmLibcNssConfigureLookupTest, RefusesADatabaseItDoesNotKnow) {
  ASSERT_EQ(LIBC_NAMESPACE::__nss_configure_lookup("automount", "files"), -1);
  ASSERT_ERRNO_EQ(EINVAL);
  libc_errno = 0;
}
