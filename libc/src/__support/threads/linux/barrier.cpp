//===-- Implementation of Barrier class ------------- ---------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/__support/threads/linux/barrier.h"
#include "hdr/errno_macros.h"
#include "src/__support/threads/sleep.h"

namespace LIBC_NAMESPACE_DECL {

int Barrier::init(Barrier *b, const pthread_barrierattr_t *attr,
                  unsigned count) {
  if (count == 0)
    return EINVAL;

  RawMutex::init(&b->lock);
  b->expected = count;
  b->arrived = 0;
  b->round = 0;
  b->inside = 0;
  b->pshared = attr ? attr->pshared == PTHREAD_PROCESS_SHARED : false;
  return 0;
}

int Barrier::wait() {
  lock.lock(cpp::nullopt, pshared);
  FutexWordType this_round = round.load(cpp::MemoryOrder::RELAXED);
  if (++arrived == expected) {
    arrived = 0;
    round.store(this_round + 1, cpp::MemoryOrder::RELEASE);
    lock.unlock(pshared);
    round.notify_all(pshared);
    return PTHREAD_BARRIER_SERIAL_THREAD;
  }
  inside.fetch_add(1, cpp::MemoryOrder::RELAXED);
  lock.unlock(pshared);

  while (round.load(cpp::MemoryOrder::ACQUIRE) == this_round)
    round.wait(this_round, cpp::nullopt, pshared);

  // The last access to the barrier, after which destroy may proceed.
  inside.fetch_sub(1, cpp::MemoryOrder::RELEASE);
  return 0;
}

int Barrier::destroy(Barrier *b) {
  while (b->inside.load(cpp::MemoryOrder::ACQUIRE) != 0)
    sleep_briefly();
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL
