//===-- Tests for TSS API like pthread_setspecific etc. -------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/pthread/pthread_create.h"
#include "src/pthread/pthread_exit.h"
#include "src/pthread/pthread_getspecific.h"
#include "src/pthread/pthread_join.h"
#include "src/pthread/pthread_key_create.h"
#include "src/pthread/pthread_key_delete.h"
#include "src/pthread/pthread_setspecific.h"
#include "test/IntegrationTest/test.h"

#include <pthread.h>

static constexpr int THREAD_DATA_INITVAL = 0x1234;
static constexpr int THREAD_DATA_FINIVAL = 0x4321;
static constexpr int THREAD_RUN_VAL = 0x600D;

static int child_thread_data = THREAD_DATA_INITVAL;
static int main_thread_data = THREAD_DATA_INITVAL;

static pthread_key_t key;
static void dtor(void *data) {
  auto *v = reinterpret_cast<int *>(data);
  *v = THREAD_DATA_FINIVAL;
}

// Used to test that we don't call the destructor when the mapped value in NULL.
static void dtor_failure(void *) { ASSERT_TRUE(false); }

static void *func(void *obj) {
  ASSERT_EQ(LIBC_NAMESPACE::pthread_setspecific(key, &child_thread_data), 0);
  int *d = reinterpret_cast<int *>(LIBC_NAMESPACE::pthread_getspecific(key));
  ASSERT_TRUE(d != nullptr);
  ASSERT_EQ(&child_thread_data, d);
  ASSERT_EQ(*d, THREAD_DATA_INITVAL);
  *reinterpret_cast<int *>(obj) = THREAD_RUN_VAL;
  return nullptr;
}

static void *func_null_val(void *) {
  // null value, we should not call dtor
  ASSERT_EQ(LIBC_NAMESPACE::pthread_setspecific(key, nullptr), 0);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_getspecific(key), nullptr);
  return nullptr;
}

static void standard_usage_test() {
  ASSERT_EQ(LIBC_NAMESPACE::pthread_key_create(&key, &dtor), 0);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_setspecific(key, &main_thread_data), 0);
  int *d = reinterpret_cast<int *>(LIBC_NAMESPACE::pthread_getspecific(key));
  ASSERT_TRUE(d != nullptr);
  ASSERT_EQ(&main_thread_data, d);
  ASSERT_EQ(*d, THREAD_DATA_INITVAL);

  pthread_t th;
  int arg = 0xBAD;
  ASSERT_EQ(LIBC_NAMESPACE::pthread_create(&th, nullptr, &func, &arg), 0);
  void *retval = &child_thread_data; // Init to some non-nullptr val.
  ASSERT_EQ(LIBC_NAMESPACE::pthread_join(th, &retval), 0);
  ASSERT_EQ(retval, nullptr);
  ASSERT_EQ(arg, THREAD_RUN_VAL);
  ASSERT_EQ(child_thread_data, THREAD_DATA_FINIVAL);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_key_delete(key), 0);
}

static void null_value_test() {
  pthread_t th;
  ASSERT_EQ(LIBC_NAMESPACE::pthread_key_create(&key, &dtor_failure), 0);
  ASSERT_EQ(
      LIBC_NAMESPACE::pthread_create(&th, nullptr, &func_null_val, nullptr), 0);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_join(th, nullptr), 0);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_key_delete(key), 0);
}

// A destructor is allowed to put its value back, and is then called again, up
// to PTHREAD_DESTRUCTOR_ITERATIONS times. That is how something which has to
// run after every other destructor arranges to: the allocator does it so that
// it can take the cache back only once nothing else will call free.
static pthread_key_t repeat_key;
static int repeat_calls = 0;
static void *repeat_last = nullptr;

static void repeat_dtor(void *data) {
  ++repeat_calls;
  repeat_last = data;
  if (repeat_calls < 3)
    LIBC_NAMESPACE::pthread_setspecific(repeat_key, data);
}

static void *repeat_func(void *) {
  LIBC_NAMESPACE::pthread_setspecific(repeat_key, &child_thread_data);
  return nullptr;
}

static void destructor_runs_again_test() {
  ASSERT_EQ(LIBC_NAMESPACE::pthread_key_create(&repeat_key, &repeat_dtor), 0);
  pthread_t th;
  ASSERT_EQ(LIBC_NAMESPACE::pthread_create(&th, nullptr, &repeat_func, nullptr),
            0);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_join(th, nullptr), 0);
  ASSERT_EQ(repeat_calls, 3);
  ASSERT_EQ(repeat_last, reinterpret_cast<void *>(&child_thread_data));
  ASSERT_EQ(LIBC_NAMESPACE::pthread_key_delete(repeat_key), 0);
}

// A process exiting is not a thread exiting: the destructor of a value the
// main thread still holds is not run. The key is deliberately left in place
// and the value left set, so that anything running it after main returns
// fails the test.
static void main_thread_exit_test() {
  pthread_key_t exit_key;
  ASSERT_EQ(LIBC_NAMESPACE::pthread_key_create(&exit_key, &dtor_failure), 0);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_setspecific(exit_key, &main_thread_data),
            0);
}

TEST_MAIN() {
  standard_usage_test();
  null_value_test();
  destructor_runs_again_test();
  main_thread_exit_test();
  return 0;
}
