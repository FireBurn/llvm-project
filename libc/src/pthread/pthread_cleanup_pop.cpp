//===-- Implementation of _pthread_cleanup_pop ----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/pthread/pthread_cleanup_pop.h"

#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/__support/threads/thread.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(void, _pthread_cleanup_pop,
                   (struct _pthread_cleanup_buffer * buffer, int execute)) {
  ThreadAttributes *attrib = internal::self.attrib;
  if (attrib == nullptr)
    return;
  auto *handler = reinterpret_cast<CleanupHandler *>(buffer);
  // Handlers come off in the order they went on, so anything pushed and not
  // popped inside this one goes first.
  while (attrib->cleanup_stack != nullptr && attrib->cleanup_stack != handler)
    attrib->cleanup_stack = attrib->cleanup_stack->next;
  if (attrib->cleanup_stack == handler)
    attrib->cleanup_stack = handler->next;
  if (execute != 0 && handler->routine != nullptr)
    handler->routine(handler->argument);
}

} // namespace LIBC_NAMESPACE_DECL
