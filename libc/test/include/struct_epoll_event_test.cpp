//===-- Unittests for struct epoll_event ---------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for struct epoll_event.
///
//===----------------------------------------------------------------------===//

#include "test/UnitTest/Test.h"

#include "include/llvm-libc-types/struct_epoll_event.h"

// The struct is written in terms of uint32_t, and code that includes the epoll
// header names that type itself.
TEST(LlvmLibcStructEpollEventTest, UsesUint32T) {
  epoll_event event = {};
  uint32_t events = 0x8000001d;
  event.events = events;
  ASSERT_EQ(event.events, events);
  ASSERT_TRUE(__is_same(decltype(event.events), uint32_t));
  ASSERT_EQ(sizeof(event.events), size_t(4));
}
