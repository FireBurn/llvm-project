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
// only `next`; `prev` lets a mutex be taken out without walking the list.
// Both may carry a tag in the low bit, which says the mutex they lead to
// uses a priority-inheriting futex.
struct Node {
  Node *next;
  Node *prev;
};

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
