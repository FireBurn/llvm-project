//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Robust mutex lists
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC___SUPPORT_THREADS_ROBUST_LIST_H
#define LLVM_LIBC_SRC___SUPPORT_THREADS_ROBUST_LIST_H

#include "hdr/stdint_proxy.h"
#include "src/__support/CPP/atomic.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/set_robust_list.h"
#include "src/__support/macros/config.h"
#include "src/__support/threads/robust_list_types.h"
#include "src/__support/threads/thread_attributes.h"

namespace LIBC_NAMESPACE_DECL {
namespace robust {

// The bits of a futex word the kernel knows about when it handles a robust
// mutex. The rest of the word is the thread id of the owner.
LIBC_INLINE_VAR constexpr uint32_t WAITERS = 0x80000000u;
LIBC_INLINE_VAR constexpr uint32_t OWNER_DIED = 0x40000000u;
LIBC_INLINE_VAR constexpr uint32_t TID_MASK = 0x3fffffffu;

// Where the futex word is, counting from the list node in the same mutex:
// the node is the sixteen bytes after the lock count, which come last in
// pthread_mutex_t, and the word is the first.
LIBC_INLINE_VAR constexpr long FUTEX_OFFSET = -24;

LIBC_INLINE Node *tagged(Node *node, bool priority_inherit) {
  return reinterpret_cast<Node *>(reinterpret_cast<uintptr_t>(node) |
                                  (priority_inherit ? 1 : 0));
}

LIBC_INLINE Node *untagged(Node *node) {
  return reinterpret_cast<Node *>(reinterpret_cast<uintptr_t>(node) &
                                  ~static_cast<uintptr_t>(1));
}

// The kernel reads the list from the thread it belongs to, as it exits, so
// the updates only have to be ordered against this thread's own code.
LIBC_INLINE void order() { __atomic_signal_fence(__ATOMIC_SEQ_CST); }

// Tells the kernel where this thread's list is. The list is empty.
LIBC_INLINE void register_list(ThreadAttributes *attrib) {
  Head &head = attrib->robust_list;
  head.next = reinterpret_cast<Node *>(&head);
  head.futex_offset = FUTEX_OFFSET;
  head.pending = nullptr;
  linux_syscalls::set_robust_list(&head, sizeof(Head));
}

// Puts the node at the front of the list.
LIBC_INLINE void enqueue(Head &head, Node *node, bool priority_inherit) {
  order();
  node->next = head.next;
  node->prev = reinterpret_cast<Node *>(&head);
  Node *first = untagged(head.next);
  if (first != reinterpret_cast<Node *>(&head))
    first->prev = node;
  order();
  head.next = tagged(node, priority_inherit);
  order();
}

// Takes the node out of the list, wherever it is.
LIBC_INLINE void dequeue(Head &head, Node *node) {
  order();
  Node *prev = node->prev;
  Node *next = untagged(node->next);
  // `prev` is the head when the node is first, and the head's `next` is where
  // a node's is.
  prev->next = node->next;
  if (next != reinterpret_cast<Node *>(&head))
    next->prev = prev;
  order();
  node->next = nullptr;
  node->prev = nullptr;
}

// The child of a fork has one thread, a copy of the one which forked, with a
// new id and no list registered. Registers it, and gives the mutexes the list
// holds, which the child owns now, its id.
LIBC_INLINE void reset_after_fork(ThreadAttributes *attrib, uint32_t parent_tid,
                                  uint32_t child_tid) {
  Head &head = attrib->robust_list;
  linux_syscalls::set_robust_list(&head, sizeof(Head));
  for (Node *node = untagged(head.next);
       node != reinterpret_cast<Node *>(&head); node = untagged(node->next)) {
    auto *word = reinterpret_cast<cpp::Atomic<uint32_t> *>(
        reinterpret_cast<char *>(node) + FUTEX_OFFSET);
    uint32_t value = word->load(cpp::MemoryOrder::RELAXED);
    if ((value & TID_MASK) == parent_tid)
      word->store((value & ~TID_MASK) | child_tid, cpp::MemoryOrder::RELAXED);
  }
}

} // namespace robust
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC___SUPPORT_THREADS_ROBUST_LIST_H
