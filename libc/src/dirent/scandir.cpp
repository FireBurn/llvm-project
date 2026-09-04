//===-- Implementation of scandir -----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/dirent/scandir.h"

#include "hdr/errno_macros.h"
#include "hdr/func/free.h"
#include "hdr/func/malloc.h"
#include "hdr/func/realloc.h"
#include "hdr/types/struct_dirent.h"
#include "src/__support/File/dir.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"

#include <dirent.h>

namespace LIBC_NAMESPACE_DECL {

namespace {

// Copies one entry out of the buffer readdir hands back, which the next
// read would otherwise overwrite.
struct ::dirent *copy_entry(struct ::dirent *entry) {
  // d_reclen is how much of the buffer this entry occupies, name included.
  size_t size = entry->d_reclen;
  if (size < sizeof(struct ::dirent))
    size = sizeof(struct ::dirent);
  auto *copy = reinterpret_cast<struct ::dirent *>(::malloc(size));
  if (copy == nullptr)
    return nullptr;
  __builtin_memcpy(copy, entry, size);
  return copy;
}

void free_all(struct ::dirent **entries, size_t count) {
  for (size_t i = 0; i < count; ++i)
    ::free(entries[i]);
  ::free(entries);
}

// The lists a directory produces are short enough that an insertion sort
// costs less than the scratch space a merge sort would want.
void sort(struct ::dirent **entries, size_t count,
          int (*compare)(const struct ::dirent **, const struct ::dirent **)) {
  for (size_t i = 1; i < count; ++i) {
    struct ::dirent *held = entries[i];
    size_t j = i;
    while (j > 0) {
      auto *previous = const_cast<const struct ::dirent *>(entries[j - 1]);
      auto *current = const_cast<const struct ::dirent *>(held);
      if (compare(&previous, &current) <= 0)
        break;
      entries[j] = entries[j - 1];
      --j;
    }
    entries[j] = held;
  }
}

} // anonymous namespace

LLVM_LIBC_FUNCTION(int, scandir,
                   (const char *path, struct dirent ***namelist,
                    int (*select)(const struct dirent *),
                    int (*compare)(const struct dirent **,
                                   const struct dirent **))) {
  if (path == nullptr || namelist == nullptr) {
    libc_errno = EINVAL;
    return -1;
  }

  auto opened = Dir::open(path);
  if (!opened.has_value()) {
    libc_errno = opened.error();
    return -1;
  }
  Dir *dir = opened.value();

  struct ::dirent **entries = nullptr;
  size_t count = 0;
  size_t capacity = 0;

  for (;;) {
    auto read = dir->read();
    if (!read.has_value()) {
      libc_errno = read.error();
      free_all(entries, count);
      dir->close();
      return -1;
    }
    struct ::dirent *entry = read.value();
    if (entry == nullptr)
      break; // The end of the directory.

    // A caller's filter decides what is worth keeping.
    if (select != nullptr && select(entry) == 0)
      continue;

    if (count == capacity) {
      size_t next = capacity == 0 ? 16 : capacity * 2;
      auto *grown = reinterpret_cast<struct ::dirent **>(
          ::realloc(entries, next * sizeof(struct ::dirent *)));
      if (grown == nullptr) {
        libc_errno = ENOMEM;
        free_all(entries, count);
        dir->close();
        return -1;
      }
      entries = grown;
      capacity = next;
    }

    struct ::dirent *copy = copy_entry(entry);
    if (copy == nullptr) {
      libc_errno = ENOMEM;
      free_all(entries, count);
      dir->close();
      return -1;
    }
    entries[count++] = copy;
  }

  if (int err = dir->close(); err != 0) {
    libc_errno = err;
    free_all(entries, count);
    return -1;
  }

  if (compare != nullptr)
    sort(entries, count, compare);

  *namelist = entries;
  return static_cast<int>(count);
}

} // namespace LIBC_NAMESPACE_DECL
