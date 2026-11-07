//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// The list of crash details a process has registered.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC___SUPPORT_CRASH_DETAIL_H
#define LLVM_LIBC_SRC___SUPPORT_CRASH_DETAIL_H

#include "src/__support/OSUtil/linux/syscall_wrappers/mmap.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/__support/threads/raw_mutex.h"

#include <stddef.h>
#include <sys/mman.h>

namespace LIBC_NAMESPACE_DECL {
namespace crash_detail {

// The layout is the one a crash handler reads from the memory of the process
// that crashed, so it is the one bionic's debuggerd expects.
struct Detail {
  const char *name;
  size_t name_size;
  const char *data;
  size_t data_size;
  Detail *prev_free;
};

LIBC_INLINE_VAR constexpr size_t DETAILS_PER_PAGE = 128;

struct Page {
  Page *prev;
  size_t used;
  Detail details[DETAILS_PER_PAGE];
};

struct State {
  RawMutex lock;
  Page *page = nullptr;
  Detail *free = nullptr;
};

LIBC_INLINE State &state() {
  static State s;
  return s;
}

// The newest page, which is the one a crash handler starts from.
LIBC_INLINE const Page *newest_page() { return state().page; }

// Hands out a detail with the given contents, or null when there is no memory
// for one. The name and the data are not copied.
LIBC_INLINE Detail *add(const void *name, size_t name_size, const void *data,
                        size_t data_size) {
  State &s = state();
  s.lock.lock();
  Detail *detail = s.free;
  if (detail != nullptr) {
    s.free = detail->prev_free;
  } else {
    if (s.page == nullptr || s.page->used == DETAILS_PER_PAGE) {
      auto memory =
          linux_syscalls::mmap(nullptr, sizeof(Page), PROT_READ | PROT_WRITE,
                               MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
      if (!memory.has_value()) {
        s.lock.unlock();
        return nullptr;
      }
      Page *page = static_cast<Page *>(memory.value());
      page->prev = s.page;
      page->used = 0;
      s.page = page;
    }
    detail = &s.page->details[s.page->used++];
  }
  detail->name = static_cast<const char *>(name);
  detail->name_size = name_size;
  detail->data = static_cast<const char *>(data);
  detail->data_size = data_size;
  detail->prev_free = nullptr;
  s.lock.unlock();
  return detail;
}

// Gives a detail back; a crash handler skips the ones with no data.
LIBC_INLINE void remove(Detail *detail) {
  State &s = state();
  s.lock.lock();
  detail->name = nullptr;
  detail->name_size = 0;
  detail->data = nullptr;
  detail->data_size = 0;
  detail->prev_free = s.free;
  s.free = detail;
  s.lock.unlock();
}

LIBC_INLINE void replace_data(Detail *detail, const void *data, size_t size) {
  State &s = state();
  s.lock.lock();
  detail->data = static_cast<const char *>(data);
  detail->data_size = size;
  s.lock.unlock();
}

LIBC_INLINE void replace_name(Detail *detail, const void *name, size_t size) {
  State &s = state();
  s.lock.lock();
  detail->name = static_cast<const char *>(name);
  detail->name_size = size;
  s.lock.unlock();
}

} // namespace crash_detail
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC___SUPPORT_CRASH_DETAIL_H
