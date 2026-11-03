//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the declaration of the name service switch configuration.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC___SUPPORT_NSS_CONFIG_H
#define LLVM_LIBC_SRC___SUPPORT_NSS_CONFIG_H

#include "src/__support/CPP/string_view.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {
namespace nss {

// What a source says about a lookup. The values are the ones of glibc's enum
// nss_status, which the modules return.
enum Status : int {
  TRYAGAIN = -2,
  UNAVAIL = -1,
  NOTFOUND = 0,
  SUCCESS = 1,
  // Asks the dispatcher to stop with what it has. Only modules say this.
  RETURN = 2,
};

enum class Action : unsigned char { CONTINUE, RETURN, MERGE };

enum Database : int {
  PASSWD,
  GROUP,
  SHADOW,
  HOSTS,
  SERVICES,
  PROTOCOLS,
  NETWORKS,
  ETHERS,
  RPC,
  ALIASES,
  NETGROUP,
  DATABASE_COUNT,
};

const char *database_name(Database db);
bool database_from_name(cpp::string_view name, Database &db);

// One word of a line in nsswitch.conf: a service and what to do with the
// answer it gives. A source is kept as long as the process runs, because the
// callers walk the list without holding anything.
struct Source {
  char *name;
  // Indexed by status + 2, from TRYAGAIN to SUCCESS.
  Action actions[4];
  Source *next;
  // The module the service is, once it has been looked for.
  void *module;
  bool module_tried;

  LIBC_INLINE Action action(int status) const {
    return status < TRYAGAIN || status > SUCCESS ? Action::CONTINUE
                                                 : actions[status - TRYAGAIN];
  }
};

// The lists of sources for every database.
struct Configuration {
  Source *sources[DATABASE_COUNT];
};

// Reads the text of an nsswitch.conf. A database it does not mention gets
// the default for it. Returns nullptr when there is no memory.
Configuration *parse(cpp::string_view text);

// The list a database stands for where nothing says otherwise: files, and
// for hosts files then dns.
const char *default_services(Database db);

// The sources of a database, from the configuration in force. The file is
// read again when it has changed.
const Source *sources(Database db);

// Replaces the services of a database for the rest of the process, as
// __nss_configure_lookup does. Returns false for an unknown database or a
// line that cannot be read.
bool configure(cpp::string_view database, cpp::string_view services);

void TESTONLY_set_config_path(const char *path);
void TESTONLY_reset_config_path();

} // namespace nss
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC___SUPPORT_NSS_CONFIG_H
