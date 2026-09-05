//===-- Implementation of glob --------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/glob/glob.h"

#include "hdr/errno_macros.h"
#include "hdr/fnmatch_macros.h"
#include "hdr/func/free.h"
#include "hdr/func/malloc.h"
#include "hdr/func/realloc.h"
#include "hdr/glob_macros.h"
#include "hdr/limits_macros.h"
#include "hdr/sys_stat_macros.h"
#include "hdr/types/glob_t.h"
#include "src/__support/CPP/string_view.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/dirent/closedir.h"
#include "src/dirent/opendir.h"
#include "src/dirent/readdir.h"
#include "src/fnmatch/fnmatch.h"
#include "src/string/string_utils.h"
#include "src/sys/stat/stat.h"

namespace LIBC_NAMESPACE_DECL {

namespace {

using string_view = cpp::string_view;

// A growing vector of owned strings. glob hands the whole thing to the
// caller, so it is built with malloc rather than the internal allocator.
class PathList {
  char **items = nullptr;
  size_t count = 0;
  size_t capacity = 0;

public:
  size_t size() const { return count; }
  char **data() const { return items; }
  char *at(size_t i) const { return items[i]; }

  // Takes ownership of |path| on success. Returns false only when out of
  // memory, in which case the caller still owns |path|.
  bool append(char *path) {
    if (count == capacity) {
      size_t next = capacity == 0 ? 16 : capacity * 2;
      void *grown = ::realloc(items, next * sizeof(char *));
      if (grown == nullptr)
        return false;
      items = reinterpret_cast<char **>(grown);
      capacity = next;
    }
    items[count++] = path;
    return true;
  }

  void release() {
    items = nullptr;
    count = 0;
    capacity = 0;
  }

  void clear() {
    for (size_t i = 0; i < count; ++i)
      ::free(items[i]);
    ::free(items);
    release();
  }
};

char *duplicate(string_view s, char extra = '\0') {
  size_t extra_len = extra == '\0' ? 0 : 1;
  char *out = reinterpret_cast<char *>(::malloc(s.size() + extra_len + 1));
  if (out == nullptr)
    return nullptr;
  for (size_t i = 0; i < s.size(); ++i)
    out[i] = s[i];
  if (extra_len != 0)
    out[s.size()] = extra;
  out[s.size() + extra_len] = '\0';
  return out;
}

// Where the search reads from. GLOB_ALTDIRFUNC says the caller has put its
// own calls in the glob_t, and everything the walk does goes through these
// so there is only one place which decides.
struct DirSource {
  const glob_t *hooks; // Null unless GLOB_ALTDIRFUNC was given.

  void *opendir(const char *path) const {
    if (hooks != nullptr)
      return hooks->gl_opendir(path);
    return LIBC_NAMESPACE::opendir(path);
  }

  struct ::dirent *readdir(void *dir) const {
    if (hooks != nullptr)
      return hooks->gl_readdir(dir);
    return LIBC_NAMESPACE::readdir(reinterpret_cast<::DIR *>(dir));
  }

  void closedir(void *dir) const {
    if (hooks != nullptr) {
      hooks->gl_closedir(dir);
      return;
    }
    LIBC_NAMESPACE::closedir(reinterpret_cast<::DIR *>(dir));
  }

  int stat(const char *path, struct stat *buf) const {
    if (hooks != nullptr)
      return hooks->gl_stat(path, buf);
    return LIBC_NAMESPACE::stat(path, buf);
  }

  bool is_directory(const char *path) const {
    struct stat buf;
    if (stat(path, &buf) != 0)
      return false;
    return S_ISDIR(buf.st_mode);
  }

  bool path_exists(const char *path) const {
    struct stat buf;
    return stat(path, &buf) == 0;
  }
};

// Whether the component holds a metacharacter, which is what decides between
// reading a directory and simply appending the name.
bool has_meta(string_view component, bool noescape) {
  for (size_t i = 0; i < component.size(); ++i) {
    char c = component[i];
    if (c == '*' || c == '?' || c == '[')
      return true;
    if (!noescape && c == '\\')
      return true;
  }
  return false;
}

// Removes the backslashes an unescaped pattern used to quote metacharacters,
// so the result can be looked up as a literal name.
char *unescape(string_view component, bool noescape) {
  if (noescape)
    return duplicate(component);
  char *out = reinterpret_cast<char *>(::malloc(component.size() + 1));
  if (out == nullptr)
    return nullptr;
  size_t len = 0;
  for (size_t i = 0; i < component.size(); ++i) {
    if (component[i] == '\\' && i + 1 < component.size())
      ++i;
    out[len++] = component[i];
  }
  out[len] = '\0';
  return out;
}

int compare(const char *a, const char *b) {
  for (; *a != '\0' && *a == *b; ++a, ++b)
    ;
  return static_cast<int>(static_cast<unsigned char>(*a)) -
         static_cast<int>(static_cast<unsigned char>(*b));
}

void sort(char **items, size_t count) {
  // The lists a glob produces are short, so an insertion sort keeps this
  // free of any allocation for scratch space.
  for (size_t i = 1; i < count; ++i) {
    char *held = items[i];
    size_t j = i;
    while (j > 0 && compare(items[j - 1], held) > 0) {
      items[j] = items[j - 1];
      --j;
    }
    items[j] = held;
  }
}

struct Walk {
  int flags;
  int (*errfunc)(const char *, int);
  DirSource source;
  bool aborted = false;
  bool out_of_memory = false;

  // Joins |prefix| and |name| into a freshly allocated path.
  char *join(string_view prefix, string_view name, bool mark_dir) {
    size_t len = prefix.size() + name.size();
    char *out = reinterpret_cast<char *>(::malloc(len + 2));
    if (out == nullptr)
      return nullptr;
    size_t at = 0;
    for (size_t i = 0; i < prefix.size(); ++i)
      out[at++] = prefix[i];
    for (size_t i = 0; i < name.size(); ++i)
      out[at++] = name[i];
    out[at] = '\0';
    if (mark_dir && (flags & GLOB_MARK) && at > 0 && out[at - 1] != '/' &&
        source.is_directory(out)) {
      out[at++] = '/';
      out[at] = '\0';
    }
    return out;
  }

  // Reports a directory which could not be read. Returns true if the walk
  // should stop.
  bool report(const char *path, int err) {
    if (errfunc != nullptr && errfunc(path, err) != 0)
      return true;
    return (flags & GLOB_ERR) != 0;
  }

  // Expands |pattern| against |prefix|, which already names an existing
  // directory and ends in '/' unless it is empty.
  void expand(string_view prefix, string_view pattern, PathList &out);
};

void Walk::expand(string_view prefix, string_view pattern, PathList &out) {
  if (aborted || out_of_memory)
    return;

  // The pattern ran out at a '/', which the prefix already carries, so the
  // prefix is the whole match. GLOB_MARK has nothing to add: the slash it
  // asks for is there. glibc appends a second one for a pattern like "t/"
  // but not for "t/sub/", which is not a distinction worth keeping.
  if (pattern.empty()) {
    char *result = duplicate(prefix);
    if (result == nullptr) {
      out_of_memory = true;
      return;
    }
    if (!source.path_exists(result)) {
      ::free(result);
      return;
    }
    if (!out.append(result)) {
      ::free(result);
      out_of_memory = true;
    }
    return;
  }

  // Split off the first component and whatever follows it.
  size_t slash = pattern.find_first_of('/');
  bool last = slash == string_view::npos;
  string_view component = last ? pattern : string_view(pattern.data(), slash);
  string_view rest = last ? string_view()
                          : string_view(pattern.data() + slash + 1,
                                        pattern.size() - slash - 1);

  // Runs of slashes belong to the prefix, not to a component. They are kept
  // as written, except at the very start where a second leading slash is
  // dropped rather than left to mean something implementation defined.
  if (component.empty()) {
    bool leading_slash_run = prefix == "/";
    char *joined =
        leading_slash_run ? duplicate(prefix) : join(prefix, "/", false);
    if (joined == nullptr) {
      out_of_memory = true;
      return;
    }
    expand(string_view(joined), rest, out);
    ::free(joined);
    return;
  }

  const bool noescape = (flags & GLOB_NOESCAPE) != 0;

  if (!has_meta(component, noescape)) {
    // A literal component is looked up rather than searched for.
    char *literal = unescape(component, noescape);
    if (literal == nullptr) {
      out_of_memory = true;
      return;
    }
    char *joined = join(prefix, literal, last);
    ::free(literal);
    if (joined == nullptr) {
      out_of_memory = true;
      return;
    }
    if (last) {
      if (!source.path_exists(joined)) {
        ::free(joined);
        return;
      }
      if (!out.append(joined))
        ::free(joined), out_of_memory = true;
      return;
    }
    // No check that this names a directory: a component which cannot be
    // read is reported by the opendir below, which is the only place
    // GLOB_ERR is about. A literal final component never reads a directory
    // at all, so a missing one is simply not a match.
    char *with_slash = duplicate(string_view(joined), '/');
    ::free(joined);
    if (with_slash == nullptr) {
      out_of_memory = true;
      return;
    }
    expand(string_view(with_slash), rest, out);
    ::free(with_slash);
    return;
  }

  // A component with a metacharacter means reading the directory.
  const char *dirname = prefix.empty() ? "." : prefix.data();
  char *owned_dirname = nullptr;
  if (!prefix.empty()) {
    owned_dirname = duplicate(prefix);
    if (owned_dirname == nullptr) {
      out_of_memory = true;
      return;
    }
    dirname = owned_dirname;
  }

  libc_errno = 0;
  void *dir = source.opendir(dirname);
  if (dir == nullptr) {
    if (report(dirname, libc_errno))
      aborted = true;
    ::free(owned_dirname);
    return;
  }

  char *pattern_text = unescape(component, /*noescape=*/true);
  if (pattern_text == nullptr) {
    source.closedir(dir);
    ::free(owned_dirname);
    out_of_memory = true;
    return;
  }
  const int fnmatch_flags =
      FNM_PERIOD | FNM_PATHNAME | (noescape ? FNM_NOESCAPE : 0);

  PathList matches;
  while (struct ::dirent *entry = source.readdir(dir)) {
    string_view name(&entry->d_name[0]);
    if (LIBC_NAMESPACE::fnmatch(pattern_text, name.data(), fnmatch_flags) != 0)
      continue;
    char *joined = join(prefix, name, last);
    if (joined == nullptr) {
      out_of_memory = true;
      break;
    }
    if (!matches.append(joined)) {
      ::free(joined);
      out_of_memory = true;
      break;
    }
  }
  source.closedir(dir);
  ::free(pattern_text);
  ::free(owned_dirname);

  if (out_of_memory) {
    matches.clear();
    return;
  }

  // Each directory's own matches are ordered, which is what makes the whole
  // result sorted without a pass over it at the end.
  if (!(flags & GLOB_NOSORT))
    sort(matches.data(), matches.size());

  for (size_t i = 0; i < matches.size(); ++i) {
    if (last) {
      if (!out.append(matches.at(i))) {
        out_of_memory = true;
        break;
      }
      continue;
    }
    if (source.is_directory(matches.at(i))) {
      char *with_slash = duplicate(string_view(matches.at(i)), '/');
      if (with_slash == nullptr) {
        out_of_memory = true;
        break;
      }
      expand(string_view(with_slash), rest, out);
      ::free(with_slash);
    }
    ::free(matches.at(i));
    matches.data()[i] = nullptr;
  }
  if (last) {
    // Ownership of every element moved to |out|, or the loop stopped early.
    if (out_of_memory)
      matches.clear();
    else
      ::free(matches.data()), matches.release();
  } else {
    for (size_t i = 0; i < matches.size(); ++i)
      ::free(matches.at(i));
    ::free(matches.data());
    matches.release();
  }
}

} // anonymous namespace

LLVM_LIBC_FUNCTION(int, glob,
                   (const char *__restrict pattern, int flags,
                    int (*errfunc)(const char *, int),
                    glob_t *__restrict pglob)) {
  if (pattern == nullptr || pglob == nullptr)
    return GLOB_NOSPACE;

  const bool appending = (flags & GLOB_APPEND) != 0;
  size_t offs = (flags & GLOB_DOOFFS) ? pglob->gl_offs : 0;
  if (!appending) {
    pglob->gl_pathc = 0;
    pglob->gl_pathv = nullptr;
    if (!(flags & GLOB_DOOFFS))
      pglob->gl_offs = 0;
  }

  PathList found;
  // The caller's own directory calls are used only when it asked for them
  // and supplied all five.
  const bool alt = (flags & GLOB_ALTDIRFUNC) != 0 && pglob != nullptr &&
                   pglob->gl_opendir != nullptr &&
                   pglob->gl_readdir != nullptr &&
                   pglob->gl_closedir != nullptr && pglob->gl_stat != nullptr;
  Walk walk{flags, errfunc, DirSource{alt ? pglob : nullptr}};
  walk.expand(string_view(), string_view(pattern), found);

  if (walk.out_of_memory) {
    found.clear();
    return GLOB_NOSPACE;
  }
  if (walk.aborted) {
    found.clear();
    return GLOB_ABORTED;
  }

  // With GLOB_NOCHECK a pattern which matched nothing stands for itself.
  if (found.size() == 0 && (flags & GLOB_NOCHECK)) {
    char *literal = duplicate(string_view(pattern));
    if (literal == nullptr || !found.append(literal)) {
      ::free(literal);
      found.clear();
      return GLOB_NOSPACE;
    }
  }
  if (found.size() == 0) {
    found.clear();
    if (!appending) {
      pglob->gl_pathc = 0;
      pglob->gl_pathv = nullptr;
    }
    return GLOB_NOMATCH;
  }

  size_t previous = appending ? pglob->gl_pathc : 0;
  size_t added = found.size();
  size_t total = offs + previous + added;
  char **paths = reinterpret_cast<char **>(::realloc(
      appending ? pglob->gl_pathv : nullptr, (total + 1) * sizeof(char *)));
  if (paths == nullptr) {
    found.clear();
    return GLOB_NOSPACE;
  }
  if (!appending)
    for (size_t i = 0; i < offs; ++i)
      paths[i] = nullptr;
  for (size_t i = 0; i < added; ++i)
    paths[offs + previous + i] = found.at(i);
  paths[total] = nullptr;
  // The paths belong to pglob now, so the list must not free them.
  ::free(found.data());
  found.release();

  pglob->gl_pathv = paths;
  pglob->gl_pathc = previous + added;
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL
