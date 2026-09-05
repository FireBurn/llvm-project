//===-- A fixture that pins the zone a test runs in -------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TEST_SRC_TIME_TZFIXTURE_H
#define LLVM_LIBC_TEST_SRC_TIME_TZFIXTURE_H

#include "src/stdlib/setenv.h"
#include "src/time/tzset.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/Test.h"

// Anything that tells the local time answers differently on a machine set to
// a different zone, so a test that states what it expects has to say which
// zone it means. Greenwich is the one these expect.
template <typename Base> class TzPinned : public Base {
public:
  void SetUp() override {
    Base::SetUp();
    LIBC_NAMESPACE::setenv("TZ", "", 1);
    LIBC_NAMESPACE::tzset();
  }
};

using TzFixture = TzPinned<LIBC_NAMESPACE::testing::Test>;
using TzErrnoFixture = TzPinned<LIBC_NAMESPACE::testing::ErrnoCheckingTest>;

#endif // LLVM_LIBC_TEST_SRC_TIME_TZFIXTURE_H
