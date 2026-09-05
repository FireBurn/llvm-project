//===-- Unittests for thread cancellation ---------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "src/__support/CPP/atomic.h"
#include "src/pthread/pthread_cancel.h"
#include "src/pthread/pthread_cleanup_pop.h"
#include "src/pthread/pthread_cleanup_push.h"
#include "src/pthread/pthread_create.h"
#include "src/pthread/pthread_join.h"
#include "src/pthread/pthread_setcancelstate.h"
#include "src/pthread/pthread_setcanceltype.h"
#include "src/pthread/pthread_testcancel.h"
#include "src/unistd/close.h"
#include "src/unistd/pipe.h"
#include "src/unistd/read.h"
#include "src/unistd/write.h"
#include "test/UnitTest/Test.h"

#include <pthread.h>

namespace {

// The public macros call the functions by their plain names, which the test
// build does not have; these do the same through the namespaced ones.
#define TEST_CLEANUP_PUSH(routine, arg)                                        \
  {                                                                            \
    struct _pthread_cleanup_buffer __buffer;                                   \
    LIBC_NAMESPACE::_pthread_cleanup_push(&__buffer, (routine), (arg));

#define TEST_CLEANUP_POP(execute)                                              \
  LIBC_NAMESPACE::_pthread_cleanup_pop(&__buffer, (execute));                  \
  }

// A thread cannot be handed a context, so what each one reports is here.
LIBC_NAMESPACE::cpp::Atomic<int> cleanup_ran;
LIBC_NAMESPACE::cpp::Atomic<int> reached_loop;
LIBC_NAMESPACE::cpp::Atomic<int> past_the_end;
LIBC_NAMESPACE::cpp::Atomic<int> inner_ran;
LIBC_NAMESPACE::cpp::Atomic<int> outer_ran;
LIBC_NAMESPACE::cpp::Atomic<int> order;
int pipe_fds[2];

void reset() {
  cleanup_ran.store(0);
  reached_loop.store(0);
  past_the_end.store(0);
  inner_ran.store(0);
  outer_ran.store(0);
  order.store(0);
}

void note_cleanup(void *) { cleanup_ran.store(1); }
void note_inner(void *) { inner_ran.store(order.fetch_add(1) + 1); }
void note_outer(void *) { outer_ran.store(order.fetch_add(1) + 1); }

// Waits on a pipe nothing writes to, which is where the request finds it.
void *blocked_in_read(void *) {
  TEST_CLEANUP_PUSH(note_cleanup, nullptr);
  char c;
  LIBC_NAMESPACE::read(pipe_fds[0], &c, 1);
  past_the_end.store(1);
  TEST_CLEANUP_POP(0);
  return reinterpret_cast<void *>(1);
}

// Never blocks, so only the point it asks for gives it a chance to stop.
void *asks_to_be_cancelled(void *) {
  for (;;) {
    reached_loop.store(1);
    LIBC_NAMESPACE::pthread_testcancel();
  }
  return reinterpret_cast<void *>(1);
}

// Turns cancellation off, so the request has to wait for it to come back on
// and for a cancellation point after that.
LIBC_NAMESPACE::cpp::Atomic<int> request_sent;

void *refuses_for_a_while(void *) {
  int previous = -1;
  LIBC_NAMESPACE::pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &previous);
  reached_loop.store(previous == PTHREAD_CANCEL_ENABLE ? 1 : 2);
  // Every one of these is a cancellation point that has to be got past,
  // because cancellation is off.
  while (request_sent.load() == 0)
    LIBC_NAMESPACE::pthread_testcancel();
  LIBC_NAMESPACE::pthread_testcancel();
  LIBC_NAMESPACE::pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, nullptr);
  // And this one is where it stops.
  LIBC_NAMESPACE::pthread_testcancel();
  past_the_end.store(1);
  return reinterpret_cast<void *>(1);
}

void *nested_handlers(void *) {
  TEST_CLEANUP_PUSH(note_outer, nullptr);
  TEST_CLEANUP_PUSH(note_inner, nullptr);
  char c;
  LIBC_NAMESPACE::read(pipe_fds[0], &c, 1);
  TEST_CLEANUP_POP(0);
  TEST_CLEANUP_POP(0);
  return reinterpret_cast<void *>(1);
}

void *pops_its_own_handler(void *) {
  TEST_CLEANUP_PUSH(note_cleanup, nullptr);
  TEST_CLEANUP_POP(1);
  return reinterpret_cast<void *>(7);
}

// Cancels the thread once it has had a chance to get where it is going.
void *cancel_after_it_starts(pthread_t thread,
                             LIBC_NAMESPACE::cpp::Atomic<int> &started) {
  while (started.load() == 0)
    ;
  LIBC_NAMESPACE::pthread_cancel(thread);
  void *result = nullptr;
  LIBC_NAMESPACE::pthread_join(thread, &result);
  return result;
}

} // anonymous namespace

TEST(LlvmLibcPthreadCancelTest, CancelsAThreadBlockedInARead) {
  reset();
  ASSERT_EQ(LIBC_NAMESPACE::pipe(pipe_fds), 0);
  pthread_t thread;
  ASSERT_EQ(LIBC_NAMESPACE::pthread_create(&thread, nullptr, blocked_in_read,
                                           nullptr),
            0);
  // Nothing is written, so the thread is in the read until it is cancelled.
  LIBC_NAMESPACE::pthread_cancel(thread);
  void *result = nullptr;
  ASSERT_EQ(LIBC_NAMESPACE::pthread_join(thread, &result), 0);
  EXPECT_EQ(result, PTHREAD_CANCELED);
  EXPECT_EQ(cleanup_ran.load(), 1);
  EXPECT_EQ(past_the_end.load(), 0);
  LIBC_NAMESPACE::close(pipe_fds[0]);
  LIBC_NAMESPACE::close(pipe_fds[1]);
}

TEST(LlvmLibcPthreadCancelTest, TestCancelIsACancellationPoint) {
  reset();
  pthread_t thread;
  ASSERT_EQ(LIBC_NAMESPACE::pthread_create(&thread, nullptr,
                                           asks_to_be_cancelled, nullptr),
            0);
  void *result = cancel_after_it_starts(thread, reached_loop);
  EXPECT_EQ(result, PTHREAD_CANCELED);
}

TEST(LlvmLibcPthreadCancelTest, ARequestWaitsWhileCancellationIsOff) {
  reset();
  request_sent.store(0);
  pthread_t thread;
  ASSERT_EQ(LIBC_NAMESPACE::pthread_create(&thread, nullptr,
                                           refuses_for_a_while, nullptr),
            0);
  while (reached_loop.load() == 0)
    ;
  LIBC_NAMESPACE::pthread_cancel(thread);
  request_sent.store(1);
  void *result = nullptr;
  ASSERT_EQ(LIBC_NAMESPACE::pthread_join(thread, &result), 0);
  // It got past every point while cancellation was off, and stopped at the
  // first one after it was turned back on.
  EXPECT_EQ(reached_loop.load(), 1);
  EXPECT_EQ(past_the_end.load(), 0);
  EXPECT_EQ(result, PTHREAD_CANCELED);
}

TEST(LlvmLibcPthreadCancelTest, HandlersRunInnermostFirst) {
  reset();
  ASSERT_EQ(LIBC_NAMESPACE::pipe(pipe_fds), 0);
  pthread_t thread;
  ASSERT_EQ(LIBC_NAMESPACE::pthread_create(&thread, nullptr, nested_handlers,
                                           nullptr),
            0);
  LIBC_NAMESPACE::pthread_cancel(thread);
  void *result = nullptr;
  ASSERT_EQ(LIBC_NAMESPACE::pthread_join(thread, &result), 0);
  EXPECT_EQ(result, PTHREAD_CANCELED);
  EXPECT_EQ(inner_ran.load(), 1);
  EXPECT_EQ(outer_ran.load(), 2);
  LIBC_NAMESPACE::close(pipe_fds[0]);
  LIBC_NAMESPACE::close(pipe_fds[1]);
}

TEST(LlvmLibcPthreadCancelTest, PopRunsTheHandlerWithoutCancelling) {
  reset();
  pthread_t thread;
  ASSERT_EQ(LIBC_NAMESPACE::pthread_create(&thread, nullptr,
                                           pops_its_own_handler, nullptr),
            0);
  void *result = nullptr;
  ASSERT_EQ(LIBC_NAMESPACE::pthread_join(thread, &result), 0);
  EXPECT_EQ(cleanup_ran.load(), 1);
  EXPECT_EQ(result, reinterpret_cast<void *>(7));
}

TEST(LlvmLibcPthreadCancelTest, RejectsUnknownStateAndType) {
  EXPECT_EQ(LIBC_NAMESPACE::pthread_setcancelstate(42, nullptr), EINVAL);
  EXPECT_EQ(LIBC_NAMESPACE::pthread_setcanceltype(42, nullptr), EINVAL);
}

TEST(LlvmLibcPthreadCancelTest, StateAndTypeReportWhatTheyReplaced) {
  int previous = -1;
  ASSERT_EQ(
      LIBC_NAMESPACE::pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &previous),
      0);
  EXPECT_EQ(previous, PTHREAD_CANCEL_ENABLE);
  ASSERT_EQ(
      LIBC_NAMESPACE::pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &previous),
      0);
  EXPECT_EQ(previous, PTHREAD_CANCEL_DISABLE);

  ASSERT_EQ(LIBC_NAMESPACE::pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,
                                                  &previous),
            0);
  EXPECT_EQ(previous, PTHREAD_CANCEL_DEFERRED);
  ASSERT_EQ(
      LIBC_NAMESPACE::pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &previous),
      0);
  EXPECT_EQ(previous, PTHREAD_CANCEL_ASYNCHRONOUS);
}
