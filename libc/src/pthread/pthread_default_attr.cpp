//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// The attributes new threads get by default
///
//===----------------------------------------------------------------------===//

#include "src/pthread/pthread_default_attr.h"

#include "src/__support/CPP/mutex.h"
#include "src/__support/macros/config.h"
#include "src/__support/threads/mutex.h"
#include "src/pthread/pthread_attr.h"

namespace LIBC_NAMESPACE_DECL {

namespace {
Mutex default_attr_mutex(false, false, false, false);
pthread_attr_t current = DEFAULT_PTHREAD_ATTR;
} // anonymous namespace

pthread_attr_t default_thread_attr() {
  cpp::lock_guard lock(default_attr_mutex);
  return current;
}

void set_default_thread_attr(const pthread_attr_t &attr) {
  cpp::lock_guard lock(default_attr_mutex);
  current = attr;
}

} // namespace LIBC_NAMESPACE_DECL
