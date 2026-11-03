//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains helpers shared by the databases which ask the sources.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC___SUPPORT_NSS_LOOKUP_H
#define LLVM_LIBC_SRC___SUPPORT_NSS_LOOKUP_H

#include "hdr/errno_macros.h"
#include "hdr/types/size_t.h"
#include "src/__support/CPP/span.h"
#include "src/__support/CPP/string_view.h"
#include "src/__support/error_or.h"
#include "src/__support/macros/attributes.h"
#include "src/__support/macros/config.h"
#include "src/__support/nss/config.h"
#include "src/__support/nss/dispatch.h"
#include "src/__support/pwd/dynamic_buffer.h"

namespace LIBC_NAMESPACE_DECL {
namespace nss {

// What a lookup is reported as: an entry, no entry, or an error. A source
// that is not there is the same as an entry that is not, as it is for glibc.
// |error| is what a module put in *errnop, which says why it wanted another
// try: ERANGE for a buffer which was too small.
LIBC_INLINE ErrorOr<bool> outcome(Status status, int error) {
  if (status == SUCCESS)
    return true;
  if (status == TRYAGAIN)
    return Error(error != 0 ? error : EAGAIN);
  return false;
}

// What a file said, as a status. A file that cannot be read is unavailable,
// and the error is kept in |hard_error| so that a lookup nothing else answers
// can report it. A buffer too small asks for another try.
LIBC_INLINE Status from_file(ErrorOr<bool> result, int &error,
                             int &hard_error) {
  if (!result.has_value()) {
    if (result.error() == ERANGE) {
      error = ERANGE;
      return TRYAGAIN;
    }
    hard_error = result.error();
    return UNAVAIL;
  }
  return result.value() ? SUCCESS : NOTFOUND;
}

LIBC_INLINE ErrorOr<bool> finish(Status status, int error, int hard_error) {
  if (error == ERANGE)
    return Error(ERANGE);
  if (status != SUCCESS && status != TRYAGAIN && hard_error != 0)
    return Error(hard_error);
  return outcome(status, error);
}

// Calls the function of a module which looks an entry up by a key, with the
// arguments glibc gives it: the key, the entry, the buffer and where to put
// an error number.
template <typename Entry, typename Key>
Status call_get(void *fn, Key key, Entry *entry, cpp::span<char> buffer,
                int &error) {
  using Fn = int (*)(Key, Entry *, char *, size_t, int *);
  int module_error = 0;
  auto status = static_cast<Status>(reinterpret_cast<Fn>(fn)(
      key, entry, buffer.data(), buffer.size(), &module_error));
  if (status == TRYAGAIN)
    error = module_error;
  return status;
}

// The same for the one which reads the next entry.
template <typename Entry>
Status call_next(void *fn, Entry *entry, cpp::span<char> buffer, int &error) {
  using Fn = int (*)(Entry *, char *, size_t, int *);
  int module_error = 0;
  auto status = static_cast<Status>(reinterpret_cast<Fn>(fn)(
      entry, buffer.data(), buffer.size(), &module_error));
  if (status == TRYAGAIN)
    error = module_error;
  return status;
}

// The functions which open and close a module's enumeration take whether to
// keep the database open, or nothing, and are called alike.
LIBC_INLINE Status call_control(void *fn) {
  return static_cast<Status>(reinterpret_cast<int (*)(int)>(fn)(1));
}

// Looks an entry up by a key in the sources of |db|. |files| searches the
// database's file, and returns an ErrorOr<bool> as the file readers do. The
// sources which are neither the file nor a module have nothing to say.
template <typename Entry, typename Key, typename Files>
ErrorOr<bool> find(Database db, const char *function, Key key, Entry *entry,
                   cpp::span<char> buffer, Files files) {
  int error = 0;
  int hard_error = 0;
  // A buffer too small ends the walk, as it does for glibc: the caller can
  // only ask again with a larger one, and a source after it would hide that.
  auto status = lookup(
      db, function,
      [&] {
        auto status = from_file(files(), error, hard_error);
        return error == ERANGE ? RETURN : status;
      },
      [] { return UNAVAIL; },
      [&](void *fn) {
        auto status = call_get<Entry, Key>(fn, key, entry, buffer, error);
        return error == ERANGE ? RETURN : status;
      });
  return finish(status, error, hard_error);
}

// Reads the next entry of |db| from the file and then from each module in
// turn.
template <typename Entry, typename Files>
ErrorOr<bool> next(Cursor &cursor, Database db, const char *setent,
                   const char *getent, const char *endent, Entry *entry,
                   cpp::span<char> buffer, Files files) {
  int error = 0;
  int hard_error = 0;
  auto status = next_entry(
      cursor, db, setent, getent, endent,
      [&] { return from_file(files(), error, hard_error); },
      [&](void *fn, bool is_control) {
        return is_control ? call_control(fn)
                          : call_next<Entry>(fn, entry, buffer, error);
      });
  return finish(status, error, hard_error);
}

// Ends the walk, telling the module which was being read.
LIBC_INLINE void reset(Cursor &cursor, const char *endent) {
  end_entries(cursor, endent, [](void *fn, bool) { return call_control(fn); });
}

// A name as a string a module can be given. The names are passed as views
// because that is what the files are searched with, and the modules take C
// strings.
class CString {
  static constexpr size_t LIMIT = 1025;
  char text[LIMIT];
  bool fits = true;

public:
  LIBC_INLINE explicit CString(cpp::string_view name) {
    if (name.size() >= LIMIT) {
      fits = false;
      text[0] = '\0';
      return;
    }
    for (size_t i = 0; i < name.size(); ++i)
      text[i] = name[i];
    text[name.size()] = '\0';
  }
  LIBC_INLINE bool ok() const { return fits; }
  LIBC_INLINE const char *get() const { return text; }
};

// Runs |attempt|, which fills the entry out of the span it is given, again
// with a buffer twice the size for as long as it fails for lack of room. The
// functions which keep their own buffer use this where the reentrant ones
// hand the failure to the caller.
template <typename F>
ErrorOr<bool> with_growing_buffer(pwd::DynamicBuffer &buffer, F attempt) {
  constexpr size_t LIMIT = 1 << 20;
  if (buffer.capacity() == 0 && !buffer.grow())
    return Error(ENOMEM);
  while (true) {
    auto result = attempt(buffer.span());
    if (result.has_value() || result.error() != ERANGE)
      return result;
    if (buffer.capacity() >= LIMIT || !buffer.grow())
      return result;
  }
}

} // namespace nss
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC___SUPPORT_NSS_LOOKUP_H
