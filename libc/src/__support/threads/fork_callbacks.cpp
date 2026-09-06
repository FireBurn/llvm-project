//===-- Implementation of at-fork callback helpers  -----------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "fork_callbacks.h"

#include "src/__support/CPP/mutex.h" // lock_guard
#include "src/__support/macros/config.h"
#include "src/__support/threads/mutex.h"

#include <stddef.h> // For size_t

namespace LIBC_NAMESPACE_DECL {

namespace {

struct ForkCallbackTriple {
  ForkCallback *prepare = nullptr;
  ForkCallback *parent = nullptr;
  ForkCallback *child = nullptr;
};

class AtForkCallbackManager {
  static constexpr size_t CALLBACK_SIZE = 32;
  // TODO: Replace this with block store when integration tests
  // can use allocators.
  ForkCallbackTriple list[CALLBACK_SIZE];
  Mutex mtx;
  size_t next_index;

public:
  constexpr AtForkCallbackManager()
      : mtx(/*is_priority_inherit=*/false, /*is_recursive=*/false,
            /*is_robust=*/false, /*is_pshared=*/false),
        next_index(0) {}

  bool register_triple(const ForkCallbackTriple &triple) {
    cpp::lock_guard lock(mtx);
    if (next_index >= CALLBACK_SIZE)
      return false;
    list[next_index] = triple;
    ++next_index;
    return true;
  }

  // The lock is taken here and held across the fork itself, so that two
  // threads cannot be forking at the same time. Without that, one thread's
  // child can inherit the lock in the state the other thread left it, and
  // then wait for a thread which does not exist in the child.
  void invoke_prepare() {
    mtx.lock();
    for (size_t i = 0; i < next_index; ++i) {
      auto prepare = list[i].prepare;
      if (prepare)
        prepare();
    }
  }

  void invoke_parent() {
    for (size_t i = 0; i < next_index; ++i) {
      auto parent = list[i].parent;
      if (parent)
        parent();
    }
    mtx.unlock();
  }

  // The child has one thread, and the lock it inherited is held by a thread
  // which is not in it, so it is put back to how it started rather than
  // unlocked.
  void invoke_child() {
    for (size_t i = 0; i < next_index; ++i) {
      auto child = list[i].child;
      if (child)
        child();
    }
    reset();
  }

  // Also for the fork which never happened, where the lock has to go back
  // without any callback having run.
  void release() { mtx.unlock(); }

  void reset() { Mutex::init(&mtx); }
};

AtForkCallbackManager cb_manager;

} // Anonymous namespace

bool register_atfork_callbacks(ForkCallback *prepare_cb,
                               ForkCallback *parent_cb,
                               ForkCallback *child_cb) {
  return cb_manager.register_triple({prepare_cb, parent_cb, child_cb});
}

void invoke_child_callbacks() { cb_manager.invoke_child(); }

void invoke_prepare_callbacks() { cb_manager.invoke_prepare(); }

void invoke_parent_callbacks() { cb_manager.invoke_parent(); }

void release_atfork_lock() { cb_manager.release(); }

} // namespace LIBC_NAMESPACE_DECL
