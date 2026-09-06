//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Generic flat-file database template engine.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC___SUPPORT_FLAT_FILE_DB_H
#define LLVM_LIBC_SRC___SUPPORT_FLAT_FILE_DB_H

#include "hdr/errno_macros.h"
#include "hdr/stdio_macros.h"
#include "hdr/types/size_t.h"
#include "src/__support/CPP/functional.h"
#include "src/__support/CPP/optional.h"
#include "src/__support/CPP/span.h"
#include "src/__support/File/file.h"
#include "src/__support/error_or.h"
#include "src/__support/macros/attributes.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {
namespace internal {

struct ReadLineResult {
  size_t bytes_read;
  bool truncated;
  // Nothing at all was there to read. A line holding only its newline reads
  // as no bytes too, so the two have to be told apart: a blank line in the
  // middle of a file is not the end of it.
  bool at_end;
};

// Forward declaration of record parser for flat database files.
template <typename EntryType>
bool parse_line(cpp::span<char> line, EntryType *entry);

// Generic flat colon-delimited database engine.
template <typename EntryType> class FlatFileDatabase {
public:
  using Matcher = cpp::function<bool(const EntryType &)>;

private:
  const char *file_path;
  File *file = nullptr;

  // Reads a single line from the given file into the provided buffer, stripping
  // any trailing '\n' and ensuring the result is null-terminated.
  // Note: POSIX getline/getdelim cannot be used here because user database
  // lookups (including reentrant _r variants) must operate in-place within a
  // fixed, bounded buffer without dynamic heap allocations or realloc.
  LIBC_INLINE static ErrorOr<ReadLineResult> read_line(File *f,
                                                       cpp::span<char> buf) {
    if (!f || buf.size() < 2)
      return Error(EINVAL);

    File::FileLock lock(f);
    size_t bytes_read = 0;
    FileIOResult result(0);
    bool truncated = false;

    for (char &ch : buf.first(buf.size() - 1)) {
      result = f->read_unlocked(&ch, 1);
      if (result.has_error())
        return Error(result.error);
      if (result.value != 1)
        break;
      ++bytes_read;
      if (ch == '\n')
        break;
    }

    auto read_span = buf.first(bytes_read);
    if (result.value == 1 && !read_span.empty() && read_span.back() != '\n') {
      truncated = true;
      char c = '\0';
      while (true) {
        result = f->read_unlocked(&c, 1);
        if (result.has_error())
          return Error(result.error);
        if (result.value != 1 || c == '\n')
          break;
      }
    }

    if (f->error_unlocked())
      return Error(EIO);

    const bool at_end = bytes_read == 0;

    // If the line ended with a newline, strip it.
    if (!read_span.empty() && read_span.back() == '\n')
      --bytes_read;

    buf[bytes_read] = '\0';
    return ReadLineResult{bytes_read, truncated, at_end};
  }

public:
  LIBC_INLINE constexpr explicit FlatFileDatabase(const char *path)
      : file_path(path) {}

  // Which file the database reads.
  LIBC_INLINE const char *path() const { return file_path; }

  // Sets or overrides the file path for database operations.
  LIBC_INLINE void set_path(const char *path) {
    if (!path)
      return;
    if (file) {
      file->close();
      file = nullptr;
    }
    file_path = path;
  }

  // Opens or rewinds the database file stream.
  LIBC_INLINE ErrorOr<void> setdb() {
    if (!file) {
      auto result = openfile(file_path, "r");
      if (!result.has_value())
        return Error(result.error());
      file = result.value();
      return {};
    }
    auto result = file->seek(0, SEEK_SET);
    if (!result.has_value())
      return Error(result.error());
    return {};
  }

  // Closes the database file stream.
  LIBC_INLINE ErrorOr<void> enddb() {
    if (file) {
      int result = file->close();
      file = nullptr;
      if (result != 0)
        return Error(result);
    }
    return {};
  }

  // Reads the next line into |buffer|, without parsing it. Returns how many
  // bytes it holds, or nothing at the end of the file. A line of its own
  // holds no bytes either, which is why the end is reported separately. This
  // is for a caller which parses with something of its own, such as one which
  // has to be reentrant and cannot use a parse which writes anywhere shared.
  LIBC_INLINE ErrorOr<cpp::optional<size_t>> getline(cpp::span<char> buffer) {
    if (!file) {
      auto res = setdb();
      if (!res.has_value())
        return Error(res.error());
    }

    auto result = read_line(file, buffer);
    if (!result.has_value())
      return Error(result.error());

    ReadLineResult res = result.value();
    if (res.at_end)
      return cpp::optional<size_t>();
    if (res.truncated)
      return Error(ERANGE);
    return cpp::optional<size_t>(res.bytes_read);
  }

  // Reads and parses the next record from the database. Returns true if an
  // entry was read, false if EOF was reached, or an Error on failure.
  LIBC_INLINE ErrorOr<bool> getnext(EntryType *entry, cpp::span<char> buffer) {
    if (!entry)
      return Error(EINVAL);

    if (!file) {
      auto res = setdb();
      if (!res.has_value())
        return Error(res.error());
    }

    auto result = read_line(file, buffer);
    if (!result.has_value())
      return Error(result.error());

    ReadLineResult res = result.value();
    if (res.at_end)
      return false;

    if (res.truncated)
      return Error(ERANGE);

    // A blank line is not a record and is not the end of the file either.
    if (res.bytes_read == 0)
      return getnext(entry, buffer);

    if (parse_line(buffer.first(res.bytes_read + 1), entry))
      return true;

    return Error(EINVAL);
  }

  // Searches for a record matching a given predicate. Returns true if the
  // entry was found, false if it's missing, or an Error if lookup failed.
  LIBC_INLINE ErrorOr<bool> lookup(Matcher matcher, EntryType *entry,
                                   cpp::span<char> buffer) {
    if (!entry)
      return Error(EINVAL);

    auto res = setdb();
    if (!res.has_value())
      return Error(res.error());

    while (true) {
      auto next_res = getnext(entry, buffer);
      if (!next_res.has_value())
        return Error(next_res.error());
      if (!next_res.value())
        return false; // EOF without match
      if (matcher(*entry))
        return true;
    }
  }
};

} // namespace internal
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC___SUPPORT_FLAT_FILE_DB_H
