//===-- Implementation of _pthread_cleanup_push ---------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/pthread/pthread_cleanup_push.h"

#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/__support/threads/thread.h"

namespace LIBC_NAMESPACE_DECL {

static_assert(sizeof(struct _pthread_cleanup_buffer) >= sizeof(CleanupHandler),
              "The public cleanup buffer must hold the internal one.");

LLVM_LIBC_FUNCTION(void, _pthread_cleanup_push,
                   (struct _pthread_cleanup_buffer * buffer,
                    void (*routine)(void *), void *arg)) {
  ThreadAttributes *attrib = internal::self.attrib;
  if (attrib == nullptr)
    return;
  auto *handler = reinterpret_cast<CleanupHandler *>(buffer);
  handler->routine = routine;
  handler->argument = arg;
  handler->next = attrib->cleanup_stack;
  handler->reserved = 0;
  attrib->cleanup_stack = handler;
}

} // namespace LIBC_NAMESPACE_DECL
