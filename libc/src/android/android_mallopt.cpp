//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of android_mallopt.
///
//===----------------------------------------------------------------------===//

#include "src/android/android_mallopt.h"
#include "hdr/errno_macros.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

// The opcodes are those of bionic's private <bionic/malloc.h>.
namespace {
constexpr int M_INIT_ZYGOTE_CHILD_PROFILING = 1;
constexpr int M_SET_ZYGOTE_CHILD = 4;
constexpr int M_GET_PROCESS_PROFILEABLE = 9;
constexpr int M_MEMTAG_STACK_IS_ON = 11;
constexpr int M_GET_DECAY_TIME_ENABLED = 12;
} // namespace

// Scudo has no hooks, leak tracking or GWP-ASan switch to drive, so what asks
// for those fails with ENOTSUP. The zygote's calls change nothing here and
// succeed, and the questions about the heap are answered for the heap there is.
LLVM_LIBC_FUNCTION(bool, android_mallopt,
                   (int opcode, void *arg, size_t arg_size)) {
  switch (opcode) {
  case M_INIT_ZYGOTE_CHILD_PROFILING:
  case M_SET_ZYGOTE_CHILD:
    return arg == nullptr && arg_size == 0;
  case M_GET_PROCESS_PROFILEABLE:
  case M_MEMTAG_STACK_IS_ON:
  case M_GET_DECAY_TIME_ENABLED:
    if (arg == nullptr || arg_size != sizeof(bool)) {
      libc_errno = EINVAL;
      return false;
    }
    *static_cast<bool *>(arg) = false;
    return true;
  default:
    libc_errno = ENOTSUP;
    return false;
  }
}

} // namespace LIBC_NAMESPACE_DECL
