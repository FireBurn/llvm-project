//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// The file tree walk behind fts.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_FTS_FTS_WALK_H
#define LLVM_LIBC_SRC_FTS_FTS_WALK_H

#include "hdr/errno_macros.h"
#include "hdr/fcntl_macros.h"
#include "hdr/fts_macros.h"
#include "hdr/func/free.h"
#include "hdr/func/malloc.h"
#include "hdr/limits_macros.h"
#include "hdr/sys_stat_macros.h"
#include "hdr/types/DIR.h"
#include "hdr/types/FTS.h"
#include "hdr/types/FTSENT.h"
#include "hdr/types/struct_dirent.h"
#include "hdr/types/struct_stat.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/ftw/ftw_walk.h"
#include "src/string/memory_utils/inline_memcpy.h"
#include "src/string/string_utils.h"

#include <stddef.h>

namespace LIBC_NAMESPACE_DECL {
namespace fts_internal {

using ftw_internal::close_dir;
using ftw_internal::is_dot_or_dot_dot;
using ftw_internal::open_dir;
using ftw_internal::read_dir;
using ftw_internal::stat_path;

// An entry and what it points at are one allocation, so freeing the entry is
// all there is to do. The paths are never changed after the entry is made, so
// what fts_path and fts_accpath point to stays valid while the entry does.
LIBC_INLINE FTSENT *make_entry(const char *path, size_t path_length,
                               const char *name, size_t name_length) {
  constexpr size_t name_offset = offsetof(FTSENT, fts_name);
  const size_t head = (name_offset + name_length + 1 + alignof(struct stat) - 1) /
                      alignof(struct stat) * alignof(struct stat);
  const size_t total = head + sizeof(struct stat) + path_length + 1;
  FTSENT *entry = static_cast<FTSENT *>(::malloc(total));
  if (entry == nullptr) {
    libc_errno = ENOMEM;
    return nullptr;
  }
  char *bytes = reinterpret_cast<char *>(entry);
  for (size_t i = 0; i < name_offset; ++i)
    bytes[i] = 0;
  inline_memcpy(entry->fts_name, name, name_length);
  entry->fts_name[name_length] = '\0';
  entry->fts_statp = reinterpret_cast<struct stat *>(bytes + head);
  char *stored_path = bytes + head + sizeof(struct stat);
  inline_memcpy(stored_path, path, path_length);
  stored_path[path_length] = '\0';
  entry->fts_path = stored_path;
  entry->fts_accpath = stored_path;
  entry->fts_pathlen = path_length;
  entry->fts_namelen = name_length;
  entry->fts_symfd = -1;
  entry->fts_instr = FTS_NOINSTR;
  return entry;
}

LIBC_INLINE void free_list(FTSENT *entry) {
  while (entry != nullptr) {
    FTSENT *next = entry->fts_link;
    ::free(entry);
    entry = next;
  }
}

// Works out what an entry is. A link is followed when the walk is logical,
// when it is a root and FTS_COMFOLLOW is set, or when the caller asked for it
// with FTS_FOLLOW.
LIBC_INLINE void classify_entry(FTS *fts, FTSENT *entry, bool follow) {
  struct stat *info = entry->fts_statp;
  entry->fts_errno = 0;
  entry->fts_cycle = nullptr;
  entry->fts_flags &= ~FTS_SYMFOLLOW;
  if (stat_path(entry->fts_accpath, info, follow ? 0 : AT_SYMLINK_NOFOLLOW) !=
      0) {
    const int error = libc_errno;
    struct stat link_info = {};
    if (follow && error == ENOENT &&
        stat_path(entry->fts_accpath, &link_info, AT_SYMLINK_NOFOLLOW) == 0 &&
        S_ISLNK(link_info.st_mode)) {
      *info = link_info;
      entry->fts_info = FTS_SLNONE;
      return;
    }
    *info = {};
    entry->fts_errno = error;
    entry->fts_info = FTS_NS;
    return;
  }
  entry->fts_ino = info->st_ino;
  entry->fts_dev = info->st_dev;
  entry->fts_nlink = info->st_nlink;
  if (S_ISDIR(info->st_mode)) {
    entry->fts_info = FTS_D;
    // Only a followed link can lead back to a directory already on the way
    // down.
    if ((fts->fts_options & FTS_LOGICAL) || follow) {
      for (FTSENT *up = entry->fts_parent;
           up != nullptr && up->fts_level >= FTS_ROOTLEVEL; up = up->fts_parent)
        if (up->fts_ino == entry->fts_ino && up->fts_dev == entry->fts_dev) {
          entry->fts_cycle = up;
          entry->fts_info = FTS_DC;
          break;
        }
    }
  } else if (S_ISLNK(info->st_mode)) {
    entry->fts_info = FTS_SL;
  } else if (S_ISREG(info->st_mode)) {
    entry->fts_info = FTS_F;
  } else {
    entry->fts_info = FTS_DEFAULT;
  }
}

// A failed stat is recorded in the entry. It is not an error of the walk, so
// errno is left as it was.
LIBC_INLINE void classify(FTS *fts, FTSENT *entry, bool follow) {
  const int saved_errno = libc_errno;
  classify_entry(fts, entry, follow);
  libc_errno = saved_errno;
}

LIBC_INLINE bool follows(const FTS *fts, const FTSENT *entry) {
  return (fts->fts_options & FTS_LOGICAL) != 0 ||
         (entry->fts_level == FTS_ROOTLEVEL &&
          (fts->fts_options & FTS_COMFOLLOW) != 0);
}

// Puts a list in the order compare gives, by merging runs. The sort is
// stable, so entries that compare equal keep the order they were read in.
LIBC_INLINE FTSENT *sort_list(FTSENT *head, __fts_compare_func_t compare) {
  if (head == nullptr || head->fts_link == nullptr)
    return head;
  FTSENT *slow = head;
  FTSENT *fast = head->fts_link;
  while (fast != nullptr && fast->fts_link != nullptr) {
    slow = slow->fts_link;
    fast = fast->fts_link->fts_link;
  }
  FTSENT *second = slow->fts_link;
  slow->fts_link = nullptr;
  FTSENT *left = sort_list(head, compare);
  FTSENT *right = sort_list(second, compare);

  FTSENT *result = nullptr;
  FTSENT **tail = &result;
  while (left != nullptr && right != nullptr) {
    const FTSENT *a = left;
    const FTSENT *b = right;
    FTSENT **take = compare(&a, &b) <= 0 ? &left : &right;
    *tail = *take;
    *take = (*take)->fts_link;
    tail = &(*tail)->fts_link;
  }
  *tail = left != nullptr ? left : right;
  return result;
}

// Reads the entries of a directory. A name only listing is not stat'd.
LIBC_INLINE FTSENT *read_children(FTS *fts, FTSENT *parent, bool names_only) {
  ::DIR *dir = open_dir(parent->fts_accpath);
  if (dir == nullptr) {
    parent->fts_errno = libc_errno;
    return nullptr;
  }
  FTSENT *head = nullptr;
  FTSENT **tail = &head;
  const char *base = parent->fts_path;
  size_t base_length = parent->fts_pathlen;
  // A path ending in a separator already has the one to put before the name.
  const bool separator = base_length != 0 && base[base_length - 1] != '/';
  for (;;) {
    struct ::dirent *item = read_dir(dir);
    if (item == nullptr)
      break;
    const bool dot = is_dot_or_dot_dot(item->d_name);
    if (dot && (fts->fts_options & FTS_SEEDOT) == 0)
      continue;
    const size_t name_length = internal::string_length(item->d_name);
    if (base_length + (separator ? 1 : 0) + name_length >= PATH_MAX) {
      libc_errno = ENAMETOOLONG;
      free_list(head);
      close_dir(dir);
      parent->fts_errno = ENAMETOOLONG;
      return nullptr;
    }
    char path[PATH_MAX];
    inline_memcpy(path, base, base_length);
    if (separator)
      path[base_length] = '/';
    inline_memcpy(path + base_length + (separator ? 1 : 0), item->d_name,
                  name_length + 1);
    FTSENT *entry = make_entry(path, base_length + (separator ? 1 : 0) + name_length,
                               item->d_name, name_length);
    if (entry == nullptr) {
      parent->fts_errno = ENOMEM;
      free_list(head);
      close_dir(dir);
      return nullptr;
    }
    entry->fts_parent = parent;
    entry->fts_level = static_cast<short>(parent->fts_level + 1);
    if (dot) {
      entry->fts_info = FTS_DOT;
    } else if (names_only) {
      entry->fts_info = FTS_NSOK;
    } else {
      classify(fts, entry, follows(fts, entry));
    }
    *tail = entry;
    tail = &entry->fts_link;
  }
  close_dir(dir);
  if (fts->fts_compar != nullptr)
    head = sort_list(head, fts->fts_compar);
  return head;
}

// A directory reached by a link, or on another device when the walk has been
// asked to stay on one, is reported but not entered.
LIBC_INLINE bool is_left_alone(const FTS *fts, const FTSENT *entry) {
  return (fts->fts_options & FTS_XDEV) != 0 && entry->fts_dev != fts->fts_dev;
}

LIBC_INLINE void arrive(FTS *fts, FTSENT *entry) {
  fts->fts_cur = entry;
  if (entry->fts_level == FTS_ROOTLEVEL && (fts->fts_options & FTS_XDEV))
    fts->fts_dev = entry->fts_dev;
}

LIBC_INLINE FTS *open_walk(char *const *paths, int options,
                           __fts_compare_func_t compare) {
  if ((options & ~FTS_OPTIONMASK) != 0) {
    libc_errno = EINVAL;
    return nullptr;
  }
  FTS *fts = static_cast<FTS *>(::malloc(sizeof(FTS)));
  if (fts == nullptr) {
    libc_errno = ENOMEM;
    return nullptr;
  }
  *fts = {};
  fts->fts_options = options;
  fts->fts_compar = compare;
  fts->fts_rfd = -1;

  // The roots are the children of an entry above them all.
  FTSENT *top = make_entry("", 0, "", 0);
  if (top == nullptr) {
    ::free(fts);
    return nullptr;
  }
  top->fts_level = FTS_ROOTPARENTLEVEL;
  top->fts_info = FTS_INIT;

  FTSENT *head = nullptr;
  FTSENT **tail = &head;
  for (; *paths != nullptr; ++paths) {
    const size_t length = internal::string_length(*paths);
    FTSENT *root = make_entry(*paths, length, *paths, length);
    if (root == nullptr) {
      free_list(head);
      ::free(top);
      ::free(fts);
      return nullptr;
    }
    root->fts_parent = top;
    root->fts_level = FTS_ROOTLEVEL;
    if (options & FTS_NOSTAT)
      root->fts_info = FTS_NSOK;
    classify(fts, root, follows(fts, root));
    *tail = root;
    tail = &root->fts_link;
  }
  if (compare != nullptr)
    head = sort_list(head, compare);
  fts->fts_child = head;
  fts->fts_cur = top;
  return fts;
}

LIBC_INLINE FTSENT *read_next(FTS *fts) {
  FTSENT *entry = fts->fts_cur;
  if (entry == nullptr)
    return nullptr;
  const unsigned short instruction = entry->fts_instr;
  entry->fts_instr = FTS_NOINSTR;

  if (instruction == FTS_AGAIN) {
    classify(fts, entry, follows(fts, entry));
    return entry;
  }
  if (instruction == FTS_FOLLOW &&
      (entry->fts_info == FTS_SL || entry->fts_info == FTS_SLNONE)) {
    classify(fts, entry, true);
    if (entry->fts_info == FTS_D)
      entry->fts_flags |= FTS_SYMFOLLOW;
    return entry;
  }

  if (entry->fts_info == FTS_D || entry->fts_info == FTS_INIT) {
    if (entry->fts_info == FTS_D &&
        (instruction == FTS_SKIP || is_left_alone(fts, entry))) {
      free_list(fts->fts_child);
      fts->fts_child = nullptr;
      entry->fts_info = FTS_DP;
      return entry;
    }
    FTSENT *children = fts->fts_child;
    fts->fts_child = nullptr;
    if (children == nullptr && entry->fts_info == FTS_D) {
      libc_errno = 0;
      children = read_children(fts, entry, false);
      if (children == nullptr && entry->fts_errno != 0) {
        entry->fts_info = FTS_DNR;
        return entry;
      }
    }
    if (children != nullptr) {
      arrive(fts, children);
      return children;
    }
    if (entry->fts_info == FTS_INIT) {
      libc_errno = 0;
      return nullptr;
    }
    entry->fts_info = FTS_DP;
    return entry;
  }

  FTSENT *next = entry->fts_link;
  if (next != nullptr) {
    arrive(fts, next);
    ::free(entry);
    return next;
  }
  FTSENT *parent = entry->fts_parent;
  ::free(entry);
  fts->fts_cur = parent;
  if (parent->fts_level == FTS_ROOTPARENTLEVEL) {
    libc_errno = 0;
    return nullptr;
  }
  parent->fts_info = FTS_DP;
  return parent;
}

LIBC_INLINE void close_walk(FTS *fts) {
  // Everything not yet reached is on the links of the entries from here up.
  FTSENT *entry = fts->fts_cur;
  while (entry != nullptr) {
    FTSENT *parent = entry->fts_parent;
    free_list(entry);
    entry = parent;
  }
  free_list(fts->fts_child);
  ::free(fts);
}

LIBC_INLINE FTSENT *children_of(FTS *fts, int options) {
  FTSENT *entry = fts->fts_cur;
  libc_errno = 0;
  if (options != 0 && options != FTS_NAMEONLY) {
    libc_errno = EINVAL;
    return nullptr;
  }
  if (entry == nullptr || entry->fts_info == FTS_DNR ||
      entry->fts_info == FTS_ERR)
    return nullptr;
  if (entry->fts_info == FTS_INIT)
    return fts->fts_child;
  if (entry->fts_info != FTS_D)
    return nullptr;
  free_list(fts->fts_child);
  fts->fts_child = read_children(fts, entry, options == FTS_NAMEONLY);
  return fts->fts_child;
}

} // namespace fts_internal
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_FTS_FTS_WALK_H
