//===-- Tracking the child a popen stream belongs to ----------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/stdio/linux/popen_registry.h"

#include "src/__support/CPP/mutex.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {
namespace internal {

Mutex popen_mutex(false, false, false, false);
PopenEntry popen_table[MAX_POPEN_STREAMS] = {};

bool popen_remember(::FILE *stream, pid_t pid) {
  cpp::lock_guard lock(popen_mutex);
  for (size_t i = 0; i < MAX_POPEN_STREAMS; ++i) {
    if (popen_table[i].stream == nullptr) {
      popen_table[i].stream = stream;
      popen_table[i].pid = pid;
      return true;
    }
  }
  return false;
}

pid_t popen_forget(::FILE *stream) {
  cpp::lock_guard lock(popen_mutex);
  for (size_t i = 0; i < MAX_POPEN_STREAMS; ++i) {
    if (popen_table[i].stream == stream) {
      pid_t pid = popen_table[i].pid;
      popen_table[i].stream = nullptr;
      popen_table[i].pid = 0;
      return pid;
    }
  }
  return -1;
}

} // namespace internal
} // namespace LIBC_NAMESPACE_DECL
