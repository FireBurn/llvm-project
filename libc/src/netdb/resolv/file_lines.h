//===-- Reading a database file a line at a time ----------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_NETDB_RESOLV_FILE_LINES_H
#define LLVM_LIBC_SRC_NETDB_RESOLV_FILE_LINES_H

#include "hdr/fcntl_macros.h"
#include "hdr/types/size_t.h"
#include "src/__support/CPP/string_view.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/open.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/read.h"
#include "src/__support/ctype_utils.h"
#include "src/__support/macros/attributes.h"
#include "src/__support/macros/config.h"
#include "src/unistd/close.h"

namespace LIBC_NAMESPACE_DECL {
namespace resolv {

// Reads one line at a time from a file, keeping only a buffer's worth. The
// databases under /etc are written this way and are read the same way.
// There is no destructor: one of these is held for as long as an enumeration
// of a database lasts, which is longer than any scope, and a variable with a
// destructor cannot be kept at that level. Whoever opens it closes it.
class FileLines {
public:
  LIBC_INLINE bool open(const char *path) {
    close_file();
    auto fd = linux_syscalls::open(path, O_RDONLY | O_CLOEXEC, 0);
    if (!fd.has_value())
      return false;
    fd_ = fd.value();
    filled_ = at_ = 0;
    ended_ = false;
    return true;
  }

  LIBC_INLINE bool is_open() const { return fd_ >= 0; }

  LIBC_INLINE void close_file() {
    if (fd_ >= 0)
      LIBC_NAMESPACE::close(fd_);
    fd_ = -1;
    filled_ = at_ = 0;
    ended_ = false;
  }

  LIBC_INLINE bool next_line(char *line, size_t capacity) {
    size_t written = 0;
    bool any = false;
    for (;;) {
      if (at_ >= filled_) {
        if (ended_)
          break;
        auto taken = linux_syscalls::read(fd_, buffer_, sizeof(buffer_));
        if (!taken.has_value() || taken.value() == 0) {
          ended_ = true;
          break;
        }
        filled_ = static_cast<size_t>(taken.value());
        at_ = 0;
      }
      any = true;
      const char c = buffer_[at_++];
      if (c == '\n')
        break;
      if (written + 1 < capacity)
        line[written++] = c;
    }
    line[written] = '\0';
    return any;
  }

private:
  int fd_ = -1;
  // Every member is given a value here so that one of these can be a variable
  // with static storage, which the enumeration of a database needs.
  char buffer_[1024] = {};
  size_t filled_ = 0;
  size_t at_ = 0;
  bool ended_ = false;
};

// Walking a database from the front, which is what the set, get and end
// triple does between them. The file is opened on the first read; the end of
// it stays reached until the caller asks to start again, so a read past the
// end says there is nothing rather than quietly starting over.
class Enumeration {
public:
  LIBC_INLINE void rewind(const char *path, bool stay_open) {
    keep_open_ = stay_open;
    ended_ = false;
    lines_.open(path);
  }

  // Closes the file. A read after this opens it again from the front, which
  // is what closing the connection to a database has always meant.
  LIBC_INLINE void stop() {
    keep_open_ = false;
    ended_ = false;
    lines_.close_file();
  }

  // The next line that holds anything, or null at the end of the file.
  LIBC_INLINE char *next(const char *path, char *line, size_t capacity) {
    if (ended_)
      return nullptr;
    if (!lines_.is_open() && !lines_.open(path)) {
      ended_ = true;
      return nullptr;
    }
    if (lines_.next_line(line, capacity))
      return line;
    ended_ = true;
    if (!keep_open_)
      lines_.close_file();
    return nullptr;
  }

private:
  // Every member is given a value here so that one of these can be a variable
  // with static storage, which the enumeration of a database needs.
  FileLines lines_;
  bool keep_open_ = false;
  bool ended_ = false;
};

// The next run of characters that is not blank, and what follows it.
LIBC_INLINE cpp::string_view next_field(cpp::string_view &rest) {
  size_t start = 0;
  while (start < rest.size() && internal::isspace(rest[start]))
    ++start;
  size_t end = start;
  while (end < rest.size() && !internal::isspace(rest[end]))
    ++end;
  cpp::string_view field = rest.substr(start, end - start);
  rest = rest.substr(end);
  return field;
}

// Anything from a comment marker to the end of the line is for people to
// read.
LIBC_INLINE void drop_comment(char *line) {
  for (size_t i = 0; line[i] != '\0'; ++i)
    if (line[i] == '#') {
      line[i] = '\0';
      return;
    }
}

LIBC_INLINE bool equal(cpp::string_view field, const char *word) {
  size_t i = 0;
  for (; word[i] != '\0'; ++i)
    if (i >= field.size() || field[i] != word[i])
      return false;
  return i == field.size();
}

} // namespace resolv
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_NETDB_RESOLV_FILE_LINES_H
