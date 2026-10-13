//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// The kernel's view of the robust mutexes a thread holds
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC___SUPPORT_THREADS_ROBUST_LIST_TYPES_H
#define LLVM_LIBC_SRC___SUPPORT_THREADS_ROBUST_LIST_TYPES_H

#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {
namespace robust {

// A link in the list, kept inside the mutex it stands for. The kernel reads
// only the word a pointer to a node points at, which is the `next` of the
// node it leads to. Both may carry a tag in the low bit, which says the mutex
// they lead to uses a priority-inheriting futex.
//
// glibc keeps its links as a pair, `prev` then `next`, and hands the kernel
// the address of `next`. A program which looks at the list itself, as Steam's
// overlay does to check that pthreads set it up, expects the futex offset
// that comes of that, so this has the same arrangement: a node is the `next`
// word, and its `prev` is the word before it.
struct Node {
  Node *next;
};

// The two words as they sit in a mutex. A pointer to a node is a pointer to
// `node`.
struct Link {
  Node *prev;
  Node node;
};

// The word before a node, which says what leads to it, so that it can be
// taken out without walking the list.
inline Node *&prev_of(Node *node) {
  return reinterpret_cast<Node **>(node)[-1];
}

// What the kernel is given with set_robust_list, and reads when the thread
// exits: every mutex on the list whose futex word names the thread as the
// owner is marked as having lost its owner and a waiter is woken. The head
// has the layout of struct robust_list_head, and `next` is the first field of
// both the head and a node, so the head can be treated as the node in front
// of the first one.
struct Head {
  Node *next;
  long futex_offset;
  Node *pending;
};

} // namespace robust
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC___SUPPORT_THREADS_ROBUST_LIST_TYPES_H
