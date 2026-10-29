//===-- Which loaded modules a lookup can see -------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// A module opened with RTLD_LOCAL is not part of the global scope: a lookup
/// through RTLD_DEFAULT does not find its symbols, and neither does a module
/// opened later that was not linked against it. It is seen by itself and by
/// what needs it.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC___SUPPORT_ELF_SCOPE_H
#define LLVM_LIBC_SRC___SUPPORT_ELF_SCOPE_H

#include "hdr/types/size_t.h"
#include "src/__support/elf/passive_abi.h"
#include "src/__support/macros/attributes.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {
namespace elf {

namespace scope_internal {

LIBC_INLINE bool named(const char *have, const char *want) {
  if (have == nullptr)
    return false;
  for (; *have == *want; ++have, ++want)
    if (*have == '\0')
      return true;
  return false;
}

// Calls `visit` with the index of `root` and of every module it needs,
// directly or not, once each. `seen` has `set.count` entries and is cleared
// here.
template <typename F>
LIBC_INLINE void for_each_dependency(const ModuleSet &set, size_t root,
                                     bool *seen, size_t *queue, F visit) {
  for (size_t i = 0; i < set.count; ++i)
    seen[i] = false;
  size_t head = 0;
  size_t tail = 0;
  queue[tail++] = root;
  seen[root] = true;
  while (head < tail) {
    const size_t at = queue[head++];
    visit(at);
    set.modules[at].for_each_needed([&](const char *needed) {
      for (size_t i = 0; i < set.count; ++i) {
        if (seen[i] || (!named(set.modules[i].soname(), needed) &&
                        !named(set.modules[i].name(), needed)))
          continue;
        seen[i] = true;
        queue[tail++] = i;
        return;
      }
    });
  }
}

} // namespace scope_internal

// Whether any module in the set was opened RTLD_LOCAL.
LIBC_INLINE bool any_local(const ModuleSet &set) {
  for (size_t i = 0; i < set.count; ++i)
    if (set.mappings[i].local)
      return true;
  return false;
}

// Marks in `hidden` (set.count entries) the modules that `requester` cannot
// see: the local ones it does not need.
LIBC_INLINE void hide_from(const ModuleSet &set, size_t requester,
                           bool *hidden) {
  for (size_t i = 0; i < set.count; ++i)
    hidden[i] = set.mappings[i].local;
  if (!any_local(set))
    return;
  bool *seen = static_cast<bool *>(__builtin_alloca(set.count));
  size_t *queue =
      static_cast<size_t *>(__builtin_alloca(set.count * sizeof(size_t)));
  scope_internal::for_each_dependency(set, requester, seen, queue,
                                      [&](size_t i) { hidden[i] = false; });
}

// Makes `root` and what it needs part of the global scope, which is what
// opening a module already loaded with RTLD_GLOBAL does.
LIBC_INLINE void make_global(ModuleSet &set, size_t root) {
  bool *seen = static_cast<bool *>(__builtin_alloca(set.count));
  size_t *queue =
      static_cast<size_t *>(__builtin_alloca(set.count * sizeof(size_t)));
  scope_internal::for_each_dependency(
      set, root, seen, queue, [&](size_t i) { set.mappings[i].local = false; });
}

} // namespace elf
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC___SUPPORT_ELF_SCOPE_H
