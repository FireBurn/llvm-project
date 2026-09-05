//===-- The walk ftw and nftw share -----------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_FTW_FTW_WALK_H
#define LLVM_LIBC_SRC_FTW_FTW_WALK_H

#include "hdr/errno_macros.h"
#include "hdr/ftw_macros.h"
#include "hdr/func/free.h"
#include "hdr/func/malloc.h"
#include "hdr/func/realloc.h"
#include "hdr/limits_macros.h"
#include "hdr/sys_stat_macros.h"
#include "hdr/types/DIR.h"
#include "hdr/types/__ftw_func_t.h"
#include "hdr/types/__nftw_func_t.h"
#include "hdr/types/struct_FTW.h"
#include "hdr/types/struct_dirent.h"
#include "hdr/types/struct_stat.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/dirent/closedir.h"
#include "src/dirent/opendir.h"
#include "src/dirent/readdir.h"
#include "src/dirent/seekdir.h"
#include "src/dirent/telldir.h"
#include "src/string/memory_utils/inline_memcpy.h"
#include "src/string/string_utils.h"
#include "src/sys/stat/lstat.h"
#include "src/sys/stat/stat.h"

namespace LIBC_NAMESPACE_DECL {
namespace ftw_internal {

// How far down the walk may go. Each level adds at least two characters to
// the path, so nothing deeper than this can be named.
constexpr int MAX_DEPTH = PATH_MAX / 2;

// What visiting one entry led to.
enum class Step {
  Continue,     // Carry on with the next entry.
  SkipSiblings, // Leave the rest of the directory this entry is in.
  Stop,         // The callback ended the walk; its value is in the state.
  Failed        // Something went wrong; errno says what.
};

// The path being walked, built up in place so each level appends its own
// component rather than the whole path being formed again.
class PathBuffer {
public:
  LIBC_INLINE bool set(const char *path) {
    length_ = internal::string_length(path);
    if (length_ >= sizeof(buffer_))
      return false;
    inline_memcpy(buffer_, path, length_ + 1);
    // A trailing separator would give every component below it an empty one
    // to follow, so it goes, except from the root which is only that.
    while (length_ > 1 && buffer_[length_ - 1] == '/')
      buffer_[--length_] = '\0';
    return true;
  }

  // Appends a component and reports where its name begins, which is the base
  // offset the callback is given. The walk is recursive, so what to go back
  // to is the caller's to hold rather than this object's.
  LIBC_INLINE bool push(const char *name, size_t &base) {
    const size_t name_length = internal::string_length(name);
    const bool separator = length_ != 0 && buffer_[length_ - 1] != '/';
    if (length_ + name_length + (separator ? 1 : 0) >= sizeof(buffer_))
      return false;
    if (separator)
      buffer_[length_++] = '/';
    base = length_;
    inline_memcpy(buffer_ + length_, name, name_length + 1);
    length_ += name_length;
    return true;
  }

  LIBC_INLINE size_t length() const { return length_; }

  LIBC_INLINE void truncate(size_t length) {
    length_ = length;
    buffer_[length_] = '\0';
  }

  LIBC_INLINE const char *get() const { return buffer_; }

  // Where the last component of the path starts.
  LIBC_INLINE size_t base() const {
    size_t last = 0;
    for (size_t i = 0; i < length_; ++i)
      if (buffer_[i] == '/')
        last = i + 1;
    return last;
  }

private:
  char buffer_[PATH_MAX] = {};
  size_t length_ = 0;
};

// The directories a walk has already been through, kept only when symbolic
// links are followed. Without this a link back up the tree would send the
// walk round for ever.
class VisitedSet {
public:
  LIBC_INLINE ~VisitedSet() { ::free(entries_); }

  // True if this directory has been through the walk already.
  LIBC_INLINE bool seen(const struct stat &info) const {
    for (size_t i = 0; i < count_; ++i)
      if (entries_[i].device == info.st_dev && entries_[i].inode == info.st_ino)
        return true;
    return false;
  }

  LIBC_INLINE bool remember(const struct stat &info) {
    if (count_ == capacity_) {
      const size_t next = capacity_ == 0 ? 16 : capacity_ * 2;
      void *grown = ::realloc(entries_, next * sizeof(Entry));
      if (grown == nullptr)
        return false;
      entries_ = static_cast<Entry *>(grown);
      capacity_ = next;
    }
    entries_[count_].device = info.st_dev;
    entries_[count_].inode = info.st_ino;
    ++count_;
    return true;
  }

private:
  struct Entry {
    dev_t device;
    ino_t inode;
  };
  Entry *entries_ = nullptr;
  size_t count_ = 0;
  size_t capacity_ = 0;
};

// What the callback is, in whichever of the two shapes the caller asked for.
struct Callback {
  __ftw_func_t simple = nullptr;
  __nftw_func_t with_position = nullptr;

  LIBC_INLINE int operator()(const char *path, const struct stat *info,
                             int kind, struct FTW *position) const {
    if (with_position != nullptr)
      return with_position(path, info, kind, position);
    return simple(path, info, kind);
  }
};

struct Walk {
  Callback callback;
  int flags = 0;
  bool action_return_values = false;
  // How many directories may be open at once, which the caller sets.
  int descriptors = 1;
  // The device the walk started on, so FTW_MOUNT can tell where it ends.
  dev_t root_device = 0;
  // What the callback returned when it ended the walk, which is what the
  // caller is told.
  int stop_value = 0;
  // Only used when links are followed, which is the only way round a cycle.
  VisitedSet visited;
};

// Turns what the callback returned into what to do next. Without
// FTW_ACTIONRETVAL any non zero value ends the walk; with it the value says
// which of the four things to do.
LIBC_INLINE Step act_on(Walk &walk, int result, bool &skip_subtree) {
  skip_subtree = false;
  if (!walk.action_return_values) {
    if (result == 0)
      return Step::Continue;
    walk.stop_value = result;
    return Step::Stop;
  }
  switch (result) {
  case FTW_CONTINUE:
    return Step::Continue;
  case FTW_SKIP_SUBTREE:
    skip_subtree = true;
    return Step::Continue;
  case FTW_SKIP_SIBLINGS:
    return Step::SkipSiblings;
  default:
    walk.stop_value = FTW_STOP;
    return Step::Stop;
  }
}

LIBC_INLINE bool is_dot_or_dot_dot(const char *name) {
  return name[0] == '.' &&
         (name[1] == '\0' || (name[1] == '.' && name[2] == '\0'));
}

LIBC_INLINE Step visit(Walk &walk, PathBuffer &path, int level, size_t base);

// Reports every entry of an open directory, and closes it. The handle is
// closed before descending as well when the caller's limit on how many may be
// open at once has been reached, and reopened where it left off afterwards.
LIBC_INLINE Step walk_entries(Walk &walk, PathBuffer &path, ::DIR *dir,
                              int level) {
  const bool keep_open = level + 1 < walk.descriptors;
  for (;;) {
    struct ::dirent *entry = LIBC_NAMESPACE::readdir(dir);
    if (entry == nullptr) {
      LIBC_NAMESPACE::closedir(dir);
      return Step::Continue;
    }
    if (is_dot_or_dot_dot(entry->d_name))
      continue;

    const size_t here = path.length();
    size_t child_base = 0;
    if (!path.push(entry->d_name, child_base)) {
      libc_errno = ENAMETOOLONG;
      LIBC_NAMESPACE::closedir(dir);
      return Step::Failed;
    }

    long position = 0;
    if (!keep_open) {
      position = LIBC_NAMESPACE::telldir(dir);
      LIBC_NAMESPACE::closedir(dir);
      dir = nullptr;
    }

    const Step step = visit(walk, path, level + 1, child_base);
    path.truncate(here);

    if (step == Step::Failed || step == Step::Stop) {
      if (dir != nullptr)
        LIBC_NAMESPACE::closedir(dir);
      return step;
    }

    if (!keep_open) {
      dir = LIBC_NAMESPACE::opendir(path.get());
      if (dir == nullptr)
        return Step::Failed;
      LIBC_NAMESPACE::seekdir(dir, position);
    }

    if (step == Step::SkipSiblings) {
      LIBC_NAMESPACE::closedir(dir);
      return Step::Continue;
    }
  }
}

// Reports one entry and, where it is a directory the walk should enter, what
// is below it.
LIBC_INLINE Step visit(Walk &walk, PathBuffer &path, int level, size_t base) {
  struct stat info = {};
  int kind = FTW_F;
  bool is_directory = false;

  // Under FTW_PHYS a link is reported as a link. Otherwise it is followed,
  // and one whose target does not exist is reported as such.
  const bool physical = (walk.flags & FTW_PHYS) != 0;
  const int stat_result = physical ? LIBC_NAMESPACE::lstat(path.get(), &info)
                                   : LIBC_NAMESPACE::stat(path.get(), &info);
  if (stat_result != 0) {
    struct stat link_info = {};
    if (!physical && LIBC_NAMESPACE::lstat(path.get(), &link_info) == 0 &&
        S_ISLNK(link_info.st_mode)) {
      // A link with no target. Only nftw has a value that says so; to ftw it
      // is just something that could not be stat'd.
      kind = walk.callback.with_position != nullptr ? FTW_SLN : FTW_NS;
      info = link_info;
    } else {
      kind = FTW_NS;
    }
  } else if (S_ISDIR(info.st_mode)) {
    kind = FTW_D;
    is_directory = true;
  } else if (physical && S_ISLNK(info.st_mode)) {
    kind = FTW_SL;
  }

  // A directory on another file system is reported but not entered.
  if (is_directory && (walk.flags & FTW_MOUNT) != 0 &&
      info.st_dev != walk.root_device)
    is_directory = false;

  // Where links are followed, a directory reached a second time is passed
  // over rather than reported again, which is what keeps a link back up the
  // tree from sending the walk round for ever.
  if (is_directory && (walk.flags & FTW_PHYS) == 0) {
    if (walk.visited.seen(info))
      return Step::Continue;
    if (!walk.visited.remember(info)) {
      libc_errno = ENOMEM;
      return Step::Failed;
    }
  }

  if (is_directory && level + 1 >= MAX_DEPTH) {
    libc_errno = ELOOP;
    return Step::Failed;
  }

  struct FTW position = {static_cast<int>(base), level};

  ::DIR *dir = nullptr;
  if (is_directory) {
    dir = LIBC_NAMESPACE::opendir(path.get());
    if (dir == nullptr) {
      // A directory that cannot be read is reported as unreadable, which is
      // what tells the caller nothing below it was seen.
      kind = FTW_DNR;
      is_directory = false;
    }
  }

  // FTW_DEPTH asks for a directory after its contents rather than before.
  const bool report_first = !is_directory || (walk.flags & FTW_DEPTH) == 0;

  if (report_first) {
    bool skip_subtree = false;
    const Step step = act_on(
        walk, walk.callback(path.get(), &info, kind, &position), skip_subtree);
    if (step != Step::Continue || skip_subtree) {
      if (dir != nullptr)
        LIBC_NAMESPACE::closedir(dir);
      return skip_subtree && step == Step::Continue ? Step::Continue : step;
    }
  }

  if (!is_directory)
    return Step::Continue;

  const Step step = walk_entries(walk, path, dir, level);
  if (step != Step::Continue)
    return step;

  if (report_first)
    return Step::Continue;

  bool skip_subtree = false;
  return act_on(walk, walk.callback(path.get(), &info, FTW_DP, &position),
                skip_subtree);
}

// The entry point both functions share.
LIBC_INLINE int walk_tree(const char *root, Callback callback, int descriptors,
                          int flags) {
  if (root == nullptr || descriptors <= 0) {
    libc_errno = EINVAL;
    return -1;
  }

  PathBuffer path;
  if (!path.set(root)) {
    libc_errno = ENAMETOOLONG;
    return -1;
  }

  Walk walk;
  walk.callback = callback;
  walk.flags = flags;
  walk.action_return_values = (flags & FTW_ACTIONRETVAL) != 0;
  walk.descriptors = descriptors;

  // A root that is not there at all is an error rather than an entry that
  // could not be stat'd: the caller named something to walk and there is
  // nothing to walk. One that exists but cannot be read still gets reported,
  // as does a symbolic link with no target.
  struct stat root_info;
  if (((flags & FTW_PHYS) != 0
           ? LIBC_NAMESPACE::lstat(path.get(), &root_info)
           : LIBC_NAMESPACE::stat(path.get(), &root_info)) != 0) {
    const int reason = libc_errno;
    if (reason != EACCES &&
        !((flags & FTW_PHYS) == 0 &&
          LIBC_NAMESPACE::lstat(path.get(), &root_info) == 0)) {
      libc_errno = reason;
      return -1;
    }
    libc_errno = reason;
  }

  if ((flags & FTW_MOUNT) != 0)
    walk.root_device = root_info.st_dev;

  const Step step = visit(walk, path, 0, path.base());
  if (step == Step::Failed)
    return -1;
  // A callback that ended the walk has its own value reported.
  return step == Step::Stop ? walk.stop_value : 0;
}

} // namespace ftw_internal
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_FTW_FTW_WALK_H
