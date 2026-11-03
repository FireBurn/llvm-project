//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the walk of a lookup through the sources of a database.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC___SUPPORT_NSS_DISPATCH_H
#define LLVM_LIBC_SRC___SUPPORT_NSS_DISPATCH_H

#include "src/__support/CPP/string_view.h"
#include "src/__support/macros/attributes.h"
#include "src/__support/macros/config.h"
#include "src/__support/nss/config.h"
#include "src/__support/nss/module.h"

namespace LIBC_NAMESPACE_DECL {
namespace nss {

// The sources libc answers for itself.
LIBC_INLINE bool is_files(const Source &source) {
  cpp::string_view name(source.name);
  return name == "files" || name == "compat";
}

LIBC_INLINE bool is_dns(const Source &source) {
  return cpp::string_view(source.name) == "dns";
}

// Asks the sources of a database in turn until one of them settles it. The
// callables are |files|, which looks in the database's file, |dns|, which asks
// the resolver, and |module|, which is handed the function _nss_<service>_
// <function> of a service module and calls it. Each returns a Status. What
// each does with the answer is as the line of nsswitch.conf says: stop, or
// go on to the next. The status of the last source asked is the result, except
// that a source whose line says to merge a success keeps it when the sources
// after it have nothing to add.
template <typename Files, typename Dns, typename Module>
Status lookup(Database db, const char *function, Files files, Dns dns,
              Module module) {
  Status result = UNAVAIL;
  bool merged = false;
  for (Source *source = const_cast<Source *>(sources(db)); source != nullptr;
       source = source->next) {
    if (is_files(*source)) {
      result = files();
    } else if (is_dns(*source)) {
      result = dns();
    } else {
      void *fn = service_function(*source, function);
      result = fn != nullptr ? module(fn) : UNAVAIL;
    }
    if (result == RETURN || source->action(result) == Action::RETURN)
      break;
    if (result == SUCCESS && source->action(result) == Action::MERGE)
      merged = true;
  }
  if (merged && result != SUCCESS && result != TRYAGAIN)
    return SUCCESS;
  return result;
}

// Where a walk through the entries of a database has got to.
struct Cursor {
  Source *source = nullptr;
  bool begun = false;
  // Whether the module of the current source has been told to open.
  bool opened = false;
};

// Gives the next entry of a database, going through its sources in order.
// |setent| and |endent| name the functions which open and close a module's
// enumeration. |files| and |module| read one entry, from the file and from a
// module, and return SUCCESS with an entry or NOTFOUND at the end. A status
// of TRYAGAIN is passed on at once and the walk stays where it is, so that a
// caller whose buffer was too small can ask again with a larger one.
template <typename Files, typename Module>
Status next_entry(Cursor &cursor, Database db, const char *setent,
                  const char *getent, const char *endent, Files files,
                  Module module) {
  if (!cursor.begun) {
    cursor.source = const_cast<Source *>(sources(db));
    cursor.begun = true;
    cursor.opened = false;
  }
  while (cursor.source != nullptr) {
    Source &source = *cursor.source;
    Status status = UNAVAIL;
    if (is_files(source)) {
      status = files();
    } else if (!is_dns(source)) {
      if (!cursor.opened) {
        cursor.opened = true;
        if (void *open = service_function(source, setent))
          module(open, true);
      }
      void *fn = service_function(source, getent);
      status = fn != nullptr ? module(fn, false) : UNAVAIL;
    }
    if (status == SUCCESS || status == TRYAGAIN)
      return status;
    if (!is_files(source) && !is_dns(source) && cursor.opened)
      if (void *close = service_function(source, endent))
        module(close, true);
    cursor.opened = false;
    cursor.source = source.next;
  }
  return NOTFOUND;
}

// Closes the enumeration of the source a cursor stands at, and lets the next
// walk begin again at the first.
template <typename Module>
void end_entries(Cursor &cursor, const char *endent, Module module) {
  if (cursor.source != nullptr && cursor.opened && !is_files(*cursor.source) &&
      !is_dns(*cursor.source))
    if (void *close = service_function(*cursor.source, endent))
      module(close, true);
  cursor = Cursor();
}

} // namespace nss
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC___SUPPORT_NSS_DISPATCH_H
