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
#include "src/__support/threads/robust_list.h"
#if defined(__linux__) && defined(LIBC_FULL_BUILD)
#include "src/__support/threads/tcb.h"
#endif

namespace LIBC_NAMESPACE_DECL {

class Mutex final : private RawMutex {
  // Use bitfields to allow encoding more attributes.
  LIBC_PREFERED_TYPE(bool) unsigned int priority_inherit : 1;
  LIBC_PREFERED_TYPE(bool) unsigned int recursive : 1;
  LIBC_PREFERED_TYPE(bool) unsigned int robust : 1;
  LIBC_PREFERED_TYPE(bool) unsigned int pshared : 1;
  LIBC_PREFERED_TYPE(bool) unsigned int error_checking : 1;
  // Robust mutexes. The owner exited holding the mutex and the new owner has
  // not yet said the state is consistent, and the mutex was unlocked without
  // that being said, which makes it unusable.
  LIBC_PREFERED_TYPE(bool) unsigned int inconsistent : 1;
  LIBC_PREFERED_TYPE(bool) unsigned int not_recoverable : 1;

  // TLS address may not work across forked processes. Use thread id instead.
  cpp::Atomic<pid_t> owner;
  size_t lock_count;
  // Where this mutex is on the list of the robust mutexes its owner holds.
  // The unused bytes at the end of pthread_mutex_t, and robust::FUTEX_OFFSET
  // counts back from here to the futex word.
  robust::Node robust_node;

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

    if (res == MutexError::NONE || res == MutexError::OWNER_DEAD) {
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

  // The list of robust mutexes the calling thread holds, or nothing when
  // there is none to put this one on. A thread has none until it has been
  // described to the library, which is later than the loader's initialisers
  // run; they get an ordinary mutex.
  LIBC_INLINE robust::Head *robust_head() {
#if defined(__linux__) && defined(LIBC_FULL_BUILD)
    if (!this->robust)
      return nullptr;
    ThreadAttributes *attrib = get_current_thread_attrib();
    return attrib == nullptr ? nullptr : &attrib->robust_list;
#else
    return nullptr;
#endif
  }

#if defined(__linux__)
  // The kernel wakes whoever waits on a robust futex when its owner exits, and
  // does it without the private flag, so a waiter has to have waited without
  // it whether or not the mutex is shared with other processes.
  static constexpr bool kRobustShared = true;

  // Locking a robust mutex. The kernel is told which mutex is about to be
  // taken before it is, so that a thread which exits in between does not
  // leave a mutex it owns off the list.
  LIBC_INLINE MutexError
  robust_lock(robust::Head &head, cpp::optional<internal::AbsTimeout> timeout,
              bool block) {
    const FutexWordType tid = static_cast<FutexWordType>(internal::gettid());
    robust::Node *node = &robust_node;
    head.pending = robust::tagged(node, priority_inherit);
    robust::order();

    MutexError result = MutexError::NONE;
    if (priority_inherit) {
      if (this->not_recoverable) {
        result = MutexError::NOT_RECOVERABLE;
      } else if (pi_try_lock()) {
        result = MutexError::NONE;
      } else {
        // The word names no owner but is not zero when the owner exited, which
        // only the kernel takes the lock from.
        ErrorOr<int> res = block ? futex.lock_pi(timeout, kRobustShared)
                                 : futex.trylock_pi(kRobustShared);
        if (res.has_value())
          result = (futex.load(cpp::MemoryOrder::RELAXED) & robust::OWNER_DIED)
                       ? MutexError::OWNER_DEAD
                       : MutexError::NONE;
        else if (res.error() == EAGAIN)
          result = MutexError::BUSY;
        else if (res.error() == EOWNERDEAD)
          result = MutexError::OWNER_DEAD;
        else if (res.error() == ETIMEDOUT)
          result = MutexError::TIMEOUT;
        else if (res.error() == EDEADLK)
          result = MutexError::DEADLOCK;
        else if (res.error() == ENOTRECOVERABLE)
          result = MutexError::NOT_RECOVERABLE;
        else
          result = MutexError::BAD_LOCK_STATE;
      }
    } else {
      for (;;) {
        if (this->not_recoverable) {
          result = MutexError::NOT_RECOVERABLE;
          break;
        }
        FutexWordType word = futex.load(cpp::MemoryOrder::RELAXED);
        if ((word & robust::TID_MASK) == 0) {
          // Unlocked, or the owner exited. The bits that say who waits and
          // that the owner exited stay, so that the next unlock wakes the
          // waiters and the new owner learns what happened.
          FutexWordType owned =
              tid | (word & (robust::WAITERS | robust::OWNER_DIED));
          if (futex.compare_exchange_strong(word, owned,
                                            cpp::MemoryOrder::ACQUIRE,
                                            cpp::MemoryOrder::RELAXED)) {
            if (word & robust::OWNER_DIED)
              result = MutexError::OWNER_DEAD;
            break;
          }
          continue;
        }
        if (!block) {
          result = MutexError::BUSY;
          break;
        }
        if (!(word & robust::WAITERS)) {
          if (!futex.compare_exchange_strong(word, word | robust::WAITERS,
                                             cpp::MemoryOrder::RELAXED,
                                             cpp::MemoryOrder::RELAXED))
            continue;
          word |= robust::WAITERS;
        }
        ErrorOr<int> res = futex.wait(word, timeout, kRobustShared);
        if (!res.has_value() && res.error() == ETIMEDOUT) {
          result = MutexError::TIMEOUT;
          break;
        }
      }
    }

    if (result == MutexError::NONE || result == MutexError::OWNER_DEAD) {
      if (result == MutexError::OWNER_DEAD)
        this->inconsistent = true;
      robust::enqueue(head, node, priority_inherit);
    }
    robust::order();
    head.pending = nullptr;
    return result;
  }

  // Unlocking a robust mutex. A mutex whose state was not made consistent
  // after its owner exited cannot be locked again.
  LIBC_INLINE bool robust_unlock(robust::Head &head) {
    const FutexWordType tid = static_cast<FutexWordType>(internal::gettid());
    robust::Node *node = &robust_node;
    if ((futex.load(cpp::MemoryOrder::RELAXED) & robust::TID_MASK) != tid)
      return false;
    head.pending = robust::tagged(node, priority_inherit);
    robust::order();
    robust::dequeue(head, node);
    if (this->inconsistent) {
      this->inconsistent = false;
      this->not_recoverable = true;
    }
    if (priority_inherit) {
      FutexWordType expected = tid;
      if (!futex.compare_exchange_strong(expected, 0, cpp::MemoryOrder::RELEASE,
                                         cpp::MemoryOrder::RELAXED))
        futex.unlock_pi(kRobustShared);
    } else {
      FutexWordType prior = futex.exchange(0, cpp::MemoryOrder::RELEASE);
      if (prior & robust::WAITERS)
        futex.notify_one(kRobustShared);
    }
    robust::order();
    head.pending = nullptr;
    return true;
  }
#endif

  LIBC_INLINE MutexError raw_lock(cpp::optional<internal::AbsTimeout> timeout) {
#if defined(__linux__)
    if (robust::Head *head = robust_head())
      return robust_lock(*head, timeout, /*block=*/true);
    if (this->priority_inherit)
      return pi_lock(timeout);
#endif
    if (this->RawMutex::lock(timeout, this->pshared))
      return MutexError::NONE;
    return MutexError::TIMEOUT;
  }

  LIBC_INLINE MutexError raw_try_lock() {
#if defined(__linux__)
    if (robust::Head *head = robust_head())
      return robust_lock(*head, cpp::nullopt, /*block=*/false);
    if (this->priority_inherit)
      return pi_try_lock() ? MutexError::NONE : MutexError::BUSY;
#endif
    return this->RawMutex::try_lock() ? MutexError::NONE : MutexError::BUSY;
  }

  LIBC_INLINE bool raw_unlock() {
#if defined(__linux__)
    if (robust::Head *head = robust_head())
      return robust_unlock(*head);
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
        error_checking(is_error_checking), inconsistent(false),
        not_recoverable(false), owner(0), lock_count(0),
        robust_node{nullptr, nullptr} {}

  // Puts the lock back to how it started, for the one thread of a process
  // which inherited it locked by a thread the fork did not carry over. Not
  // for anything else: it does not wake whoever was waiting, because in a
  // freshly forked child there is nobody to wake.
  LIBC_INLINE static void init(Mutex *lock) {
    RawMutex::init(lock);
    lock->owner.store(0);
    lock->lock_count = 0;
    lock->robust_node = {nullptr, nullptr};
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
    return lock_impl([this] { return raw_try_lock(); });
  }

  // The owner of a robust mutex that lost its previous one says the state it
  // protects is usable again. Anything else is a mistake.
  LIBC_INLINE MutexError consistent() {
#if defined(__linux__)
    const FutexWordType tid = static_cast<FutexWordType>(internal::gettid());
    if (this->robust && this->inconsistent &&
        (futex.load(cpp::MemoryOrder::RELAXED) & robust::TID_MASK) == tid) {
      this->inconsistent = false;
      futex.fetch_and(~static_cast<FutexWordType>(robust::OWNER_DIED),
                      cpp::MemoryOrder::RELAXED);
      return MutexError::NONE;
    }
#endif
    return MutexError::BAD_LOCK_STATE;
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
