//===--- Implementation of a Unix mutex class -------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC___SUPPORT_THREADS_UNIX_MUTEX_H
#define LLVM_LIBC_SRC___SUPPORT_THREADS_UNIX_MUTEX_H

#include "hdr/types/pid_t.h"
#include "hdr/types/size_t.h"
#include "src/__support/CPP/atomic.h"
#include "src/__support/CPP/optional.h"
#include "src/__support/libc_assert.h"
#include "src/__support/macros/config.h"
#include "src/__support/macros/optimization.h"
#include "src/__support/threads/identifier.h"
#include "src/__support/threads/mutex_common.h"
#include "src/__support/threads/raw_mutex.h"

namespace LIBC_NAMESPACE_DECL {

// TODO: support shared/recursive/robust mutexes.
class Mutex final : private RawMutex {
  // Use bitfields to allow encoding more attributes.
  // TODO: the robustness will need to be implemented.
  //       See also https://github.com/llvm/llvm-project/issues/194396
  LIBC_PREFERED_TYPE(bool) unsigned int priority_inherit : 1;
  LIBC_PREFERED_TYPE(bool) unsigned int recursive : 1;
  LIBC_PREFERED_TYPE(bool) unsigned int robust : 1;
  LIBC_PREFERED_TYPE(bool) unsigned int pshared : 1;
  LIBC_PREFERED_TYPE(bool) unsigned int error_checking : 1;

  // TLS address may not work across forked processes. Use thread id instead.
  cpp::Atomic<pid_t> owner;
  size_t lock_count;

  // CndVar needs to access Mutex as RawMutex
  friend class CndVar;

  template <class LockRoutine>
  LIBC_INLINE MutexError lock_impl(LockRoutine do_lock) {
    if (is_recursive() && owner == internal::gettid()) {
      if (LIBC_UNLIKELY(lock_count == cpp::numeric_limits<size_t>::max()))
        return MutexError::OVERFLOW;
      lock_count++;
      return MutexError::NONE;
    } else if (is_error_checking() && owner == internal::gettid())
      return MutexError::DEADLOCK;

    MutexError res = do_lock();

    if (res == MutexError::NONE) {
      if (is_recursive()) {
        owner = internal::gettid();
        lock_count = 1;
      } else if (is_error_checking()) {
        owner = internal::gettid();
      }
    }

    return res;
  }

#if defined(__linux__)
  // A priority-inheriting mutex keeps its owner's thread id in the futex word
  // rather than the states RawMutex uses, since that is what the kernel reads.
  LIBC_INLINE bool pi_try_lock() {
    FutexWordType expected = 0;
    return futex.compare_exchange_strong(
        expected, static_cast<FutexWordType>(internal::gettid()),
        cpp::MemoryOrder::ACQUIRE, cpp::MemoryOrder::RELAXED);
  }

  LIBC_INLINE MutexError pi_lock(cpp::optional<internal::AbsTimeout> timeout) {
    if (pi_try_lock())
      return MutexError::NONE;
    ErrorOr<int> result = futex.lock_pi(timeout, this->pshared);
    if (result.has_value())
      return MutexError::NONE;
    if (result.error() == ETIMEDOUT)
      return MutexError::TIMEOUT;
    if (result.error() == EDEADLK)
      return MutexError::DEADLOCK;
    return MutexError::BAD_LOCK_STATE;
  }

  LIBC_INLINE bool pi_unlock() {
    FutexWordType expected = static_cast<FutexWordType>(internal::gettid());
    // Anything but the bare thread id means there are waiters for the kernel
    // to hand the lock to.
    if (futex.compare_exchange_strong(expected, 0, cpp::MemoryOrder::RELEASE,
                                      cpp::MemoryOrder::RELAXED))
      return true;
    return futex.unlock_pi(this->pshared).has_value();
  }
#endif

  LIBC_INLINE MutexError raw_lock(cpp::optional<internal::AbsTimeout> timeout) {
#if defined(__linux__)
    if (this->priority_inherit)
      return pi_lock(timeout);
#endif
    if (this->RawMutex::lock(timeout, this->pshared))
      return MutexError::NONE;
    return MutexError::TIMEOUT;
  }

  LIBC_INLINE bool raw_try_lock() {
#if defined(__linux__)
    if (this->priority_inherit)
      return pi_try_lock();
#endif
    return this->RawMutex::try_lock();
  }

  LIBC_INLINE bool raw_unlock() {
#if defined(__linux__)
    if (this->priority_inherit)
      return pi_unlock();
#endif
    return this->RawMutex::unlock(this->pshared);
  }

public:
  LIBC_INLINE constexpr Mutex(bool is_priority_inherit, bool is_recursive,
                              bool is_robust, bool is_pshared,
                              bool is_error_checking = false)
      : RawMutex(), priority_inherit(is_priority_inherit),
        recursive(is_recursive), robust(is_robust), pshared(is_pshared),
        error_checking(is_error_checking), owner(0), lock_count(0) {}

  // Puts the lock back to how it started, for the one thread of a process
  // which inherited it locked by a thread the fork did not carry over. Not
  // for anything else: it does not wake whoever was waiting, because in a
  // freshly forked child there is nobody to wake.
  LIBC_INLINE static void init(Mutex *lock) {
    RawMutex::init(lock);
    lock->owner.store(0);
    lock->lock_count = 0;
  }

  LIBC_INLINE static MutexError destroy(Mutex *lock) {
    LIBC_ASSERT(lock->owner == 0 && lock->lock_count == 0 &&
                "Mutex destroyed while being locked.");
    RawMutex::destroy(lock);
    return MutexError::NONE;
  }

  LIBC_INLINE MutexError lock() {
    return lock_impl([this] {
      // TODO: check deadlock? POSIX made it optional.
      return raw_lock(/*timeout=*/cpp::nullopt);
    });
  }

  LIBC_INLINE MutexError timed_lock(internal::AbsTimeout abs_time) {
    return lock_impl([this, abs_time] {
      // TODO: check deadlock? POSIX made it optional.
      return raw_lock(abs_time);
    });
  }

  LIBC_INLINE MutexError unlock() {
    if (is_recursive()) {
      // lock_count == 0 can happen if previous unlock is
      // suspended before signal frame
      if (owner != internal::gettid() || lock_count == 0)
        return MutexError::UNLOCK_WITHOUT_LOCK;

      lock_count--;
      if (lock_count == 0)
        owner = 0;
      else
        return MutexError::NONE;
    } else if (is_error_checking()) {
      if (owner != internal::gettid())
        return MutexError::UNLOCK_WITHOUT_LOCK;
      owner = 0;
    }
    if (raw_unlock())
      return MutexError::NONE;
    return MutexError::UNLOCK_WITHOUT_LOCK;
  }

  LIBC_INLINE MutexError try_lock() {
    return lock_impl([this] {
      if (raw_try_lock())
        return MutexError::NONE;
      return MutexError::BUSY;
    });
  }

  LIBC_INLINE bool can_be_requeued() const {
    return !this->pshared && !this->robust && !this->recursive &&
           !this->priority_inherit && !this->error_checking;
  }

  LIBC_INLINE bool is_robust() const { return this->robust; }
  LIBC_INLINE bool is_recursive() const { return this->recursive; }
  LIBC_INLINE bool is_error_checking() const { return this->error_checking; }
};

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC___SUPPORT_THREADS_UNIX_MUTEX_H
