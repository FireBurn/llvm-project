//===-- Implementation of exit --------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/stdlib/exit.h"
#include "src/__support/OSUtil/exit.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"

#ifdef LIBC_COPT_SUPPORT_THREADS
#include "src/__support/threads/thread.h"
#endif

#ifdef LIBC_COPT_EXIT_FLUSHES_STREAMS
#include "hdr/types/FILE.h"
#include "src/__support/File/file.h"
#include "src/__support/macros/attributes.h"
#endif

namespace LIBC_NAMESPACE_DECL {

// Defined with the rest of the exit handlers. Visibility is declared here as
// well as there: the parts of the library are merged into one object before
// it becomes a shared one, and the narrower of the two would otherwise win
// and keep it in.
extern "C" LIBC_SHARED_INTERNAL void __cxa_finalize(void *);

#ifdef LIBC_COPT_EXIT_FLUSHES_STREAMS
// The standard streams have static storage and were opened by nobody, so they
// never join the list of open files and flushing that list does not reach
// them. They are flushed by name instead, and referred to weakly so that a
// program which never uses one does not pull it in for the sake of this.
extern FILE *stdin [[gnu::weak]];
extern FILE *stdout [[gnu::weak]];
extern FILE *stderr [[gnu::weak]];

namespace {

// The variable itself may not be there at all, since the reference to it is
// weak, so what is checked first is whether there is one to read.
LIBC_INLINE void flush_standard_stream(FILE *const *slot) {
  if (slot == nullptr || *slot == nullptr)
    return;
  reinterpret_cast<File *>(*slot)->flush();
}

} // anonymous namespace
#endif

// TODO: use recursive mutex to protect this routine.
[[noreturn]] LLVM_LIBC_FUNCTION(void, exit, (int status)) {
#ifdef LIBC_COPT_SUPPORT_THREADS
  // Call TLS destructors, if supported by the target.
  internal::call_atexit_callbacks(current_thread().attrib);
#endif
  __cxa_finalize(nullptr);
#ifdef LIBC_COPT_EXIT_FLUSHES_STREAMS
  // C requires the streams to be flushed after the handlers have run, so a
  // handler which writes something still gets it out.
  File::flush_all();
  flush_standard_stream(&stdin);
  flush_standard_stream(&stdout);
  flush_standard_stream(&stderr);
#endif
  internal::exit(status);
}

} // namespace LIBC_NAMESPACE_DECL
