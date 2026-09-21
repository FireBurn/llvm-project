//===-- Unittests for __llvm_libc_init_main_thread ------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// A program that did not start through the startup code has nobody to
/// describe its first thread, which this does.
///
//===----------------------------------------------------------------------===//

#include "hdr/pthread_macros.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/gettid.h"
#include "src/__support/threads/thread.h"
#include "src/compat/__llvm_libc_init_main_thread.h"
#include "src/pthread/pthread_equal.h"
#include "src/pthread/pthread_self.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcInitMainThreadTest, DescribesTheThreadItIsCalledOn) {
  ASSERT_EQ(LIBC_NAMESPACE::__llvm_libc_init_main_thread(), 0);

  auto *attrib = LIBC_NAMESPACE::get_current_thread_attrib();
  ASSERT_TRUE(attrib != nullptr);
  ASSERT_EQ(static_cast<long>(attrib->tid),
            static_cast<long>(LIBC_NAMESPACE::linux_syscalls::gettid()));
  ASSERT_EQ(attrib->stacksize, static_cast<size_t>(PTHREAD_STACK_DYNAMIC_NP));
  ASSERT_TRUE(attrib->stack != nullptr);
}

TEST(LlvmLibcInitMainThreadTest, CallingItAgainLeavesTheDescriptionAlone) {
  ASSERT_EQ(LIBC_NAMESPACE::__llvm_libc_init_main_thread(), 0);
  auto *first = LIBC_NAMESPACE::get_current_thread_attrib();
  ASSERT_EQ(LIBC_NAMESPACE::__llvm_libc_init_main_thread(), 0);
  ASSERT_TRUE(first == LIBC_NAMESPACE::get_current_thread_attrib());
}

TEST(LlvmLibcInitMainThreadTest, ThreadSelfWorksAfterwards) {
  ASSERT_EQ(LIBC_NAMESPACE::__llvm_libc_init_main_thread(), 0);
  pthread_t self = LIBC_NAMESPACE::pthread_self();
  ASSERT_NE(LIBC_NAMESPACE::pthread_equal(self, self), 0);
}
