//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the tests of pthread_gettid_np.
///
//===----------------------------------------------------------------------===//

#include "src/pthread/pthread_gettid_np.h"
#include "src/pthread/pthread_self.h"
#include "src/unistd/gettid.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcPthreadGettidNpTest, MatchesGettidForTheCallingThread) {
  ASSERT_EQ(LIBC_NAMESPACE::pthread_gettid_np(LIBC_NAMESPACE::pthread_self()),
            LIBC_NAMESPACE::gettid());
}
