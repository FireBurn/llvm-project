//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Installs debuggerd's crash handler at startup, as bionic's linker does.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_ANDROID_DEBUGGERD_HOOK_H
#define LLVM_LIBC_SRC_ANDROID_DEBUGGERD_HOOK_H

#include "src/__support/abort_message.h"
#include "src/__support/crash_detail.h"
#include "src/__support/macros/attributes.h"
#include "src/__support/macros/config.h"

#include <stddef.h>

// AOSP's libdebuggerd_handler defines this and the structures it takes, which
// this mirrors from system/core/debuggerd/include/debuggerd/handler.h. A
// program that does not link the library has no such symbol, and nothing is
// installed.
namespace LIBC_NAMESPACE_DECL {
namespace debuggerd_hook {

struct __attribute__((packed)) ProcessInfo {
  void *abort_msg;
  void *fdsan_table;
  const void *gwp_asan_state;
  const void *gwp_asan_metadata;
  const char *scudo_stack_depot;
  const char *scudo_region_info;
  const char *scudo_ring_buffer;
  size_t scudo_ring_buffer_size;
  size_t scudo_stack_depot_size;
  bool recoverable_crash;
  void *crash_detail_page;
};

struct Callbacks {
  ProcessInfo (*get_process_info)();
  void (*get_gwp_asan_callbacks)();
  void (*post_dump)();
};

} // namespace debuggerd_hook
} // namespace LIBC_NAMESPACE_DECL

extern "C" [[gnu::weak]] void
debuggerd_init(LIBC_NAMESPACE::debuggerd_hook::Callbacks *callbacks);

namespace LIBC_NAMESPACE_DECL {
namespace debuggerd_hook {

// Called in the signal handler, so it only reads what is already there.
LIBC_INLINE ProcessInfo process_info() {
  ProcessInfo info = {};
  if (abort_message::get() != nullptr)
    info.abort_msg = &abort_message::block();
  info.crash_detail_page =
      const_cast<crash_detail::Page *>(crash_detail::newest_page());
  return info;
}

LIBC_INLINE void install() {
  static Callbacks callbacks = {&process_info, nullptr, nullptr};
  if (&::debuggerd_init != nullptr)
    ::debuggerd_init(&callbacks);
}

} // namespace debuggerd_hook
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_ANDROID_DEBUGGERD_HOOK_H
