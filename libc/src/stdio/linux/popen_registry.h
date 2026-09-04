//===-- Tracking the child a popen stream belongs to ------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_STDIO_LINUX_POPEN_REGISTRY_H
#define LLVM_LIBC_SRC_STDIO_LINUX_POPEN_REGISTRY_H

#include "hdr/types/FILE.h"
#include "hdr/types/pid_t.h"
#include "hdr/types/size_t.h"
#include "src/__support/macros/attributes.h"
#include "src/__support/macros/config.h"
#include "src/__support/threads/mutex.h"

namespace LIBC_NAMESPACE_DECL {
namespace internal {

// pclose has to wait for the process popen started, but the only thing it is
// given is the stream. This maps one to the other.
//
// The table is fixed size rather than allocated: popen is used by programs
// that are otherwise careful about allocation, and an open stream already
// costs a file descriptor, which is the scarcer resource.
constexpr size_t MAX_POPEN_STREAMS = 32;

struct PopenEntry {
  ::FILE *stream;
  pid_t pid;
};

extern Mutex popen_mutex;
extern PopenEntry popen_table[MAX_POPEN_STREAMS];

// Returns false when the table is full, in which case the caller must not
// leave the child running.
bool popen_remember(::FILE *stream, pid_t pid);

// Returns the child's pid and forgets the stream, or -1 if the stream did not
// come from popen.
pid_t popen_forget(::FILE *stream);

} // namespace internal
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_STDIO_LINUX_POPEN_REGISTRY_H
