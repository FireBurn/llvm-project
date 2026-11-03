//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Reading the networks database
///
//===----------------------------------------------------------------------===//

#include "src/netdb/resolv/networks.h"

#include "hdr/sys_socket_macros.h"
#include "src/__support/CPP/string_view.h"
#include "src/__support/ctype_utils.h"
#include "src/__support/macros/attributes.h"
#include "src/__support/macros/config.h"
#include "src/__support/nss/dispatch.h"
#include "src/__support/nss/lookup.h"
#include "src/netdb/resolv/file_lines.h"
#include "src/string/memory_utils/inline_memcpy.h"

namespace LIBC_NAMESPACE_DECL {
namespace resolv {

namespace {

constexpr const char *NETWORKS_PATH = "/etc/networks";

// Where the enumeration has got to. setnetent asks for the file to be held
// open across lookups, which is what the enumeration records.
LIBC_CONSTINIT LIBC_THREAD_LOCAL Enumeration enumeration;
LIBC_THREAD_LOCAL nss::Cursor cursor;

char *keep(cpp::string_view field, NetStorage &storage) {
  if (storage.used + field.size() + 1 > NetStorage::POOL_SIZE)
    return nullptr;
  char *at = storage.pool + storage.used;
  inline_memcpy(at, field.data(), field.size());
  at[field.size()] = '\0';
  storage.used += field.size() + 1;
  return at;
}

// A network number is written the way an address is, but with the parts that
// are always zero left off: 127 stands for 127.0.0.0.
bool parse_number(cpp::string_view text, uint32_t &out) {
  uint32_t parts[4] = {0, 0, 0, 0};
  size_t count = 0;
  size_t at = 0;

  while (at < text.size() && count < 4) {
    if (!internal::isdigit(text[at]))
      return false;
    uint32_t value = 0;
    while (at < text.size() && internal::isdigit(text[at])) {
      value = value * 10 + static_cast<uint32_t>(text[at] - '0');
      if (value > 255)
        return false;
      ++at;
    }
    parts[count++] = value;
    if (at == text.size())
      break;
    if (text[at] != '.')
      return false;
    ++at;
    // A trailing dot names nothing.
    if (at == text.size())
      return false;
  }
  if (at != text.size() || count == 0)
    return false;

  out = (parts[0] << 24) | (parts[1] << 16) | (parts[2] << 8) | parts[3];
  return true;
}

bool parse(char *line, NetStorage &storage) {
  drop_comment(line);

  cpp::string_view rest(line);
  cpp::string_view official = next_field(rest);
  if (official.empty())
    return false;
  cpp::string_view number = next_field(rest);
  if (number.empty())
    return false;

  uint32_t net = 0;
  if (!parse_number(number, net))
    return false;

  storage.used = 0;
  char *name = keep(official, storage);
  if (name == nullptr)
    return false;

  size_t count = 0;
  for (cpp::string_view other = next_field(rest); !other.empty();
       other = next_field(rest)) {
    if (count == NetStorage::MAX_ALIASES)
      break;
    char *kept = keep(other, storage);
    if (kept == nullptr)
      break;
    storage.aliases[count++] = kept;
  }
  storage.aliases[count] = nullptr;

  storage.entry.n_name = name;
  storage.entry.n_aliases = storage.aliases;
  storage.entry.n_addrtype = AF_INET;
  storage.entry.n_net = net;
  return true;
}

bool names_match(const NetStorage &storage, const char *name) {
  if (equal(cpp::string_view(storage.entry.n_name), name))
    return true;
  for (size_t i = 0; storage.aliases[i] != nullptr; ++i)
    if (equal(cpp::string_view(storage.aliases[i]), name))
      return true;
  return false;
}

// What a module put in the storage, as the answer. Their functions take a
// second place to put an error number, for the resolver.
nss::Status offered(nss::Status status, const NetStorage &storage) {
  if (status != nss::SUCCESS)
    return status;
  return storage.entry.n_name != nullptr ? nss::SUCCESS : nss::NOTFOUND;
}

struct netent *file_net_by_name(const char *name, NetStorage &storage) {
  FileLines reader;
  if (!reader.open(NETWORKS_PATH))
    return nullptr;
  struct netent *found = nullptr;
  char line[512];
  while (found == nullptr && reader.next_line(line, sizeof(line)))
    if (parse(line, storage) && names_match(storage, name))
      found = &storage.entry;
  reader.close_file();
  return found;
}

struct netent *file_net_by_addr(uint32_t net, int type, NetStorage &storage) {
  if (type != AF_INET)
    return nullptr;
  FileLines reader;
  if (!reader.open(NETWORKS_PATH))
    return nullptr;
  struct netent *found = nullptr;
  char line[512];
  while (found == nullptr && reader.next_line(line, sizeof(line)))
    if (parse(line, storage) && storage.entry.n_net == net)
      found = &storage.entry;
  reader.close_file();
  return found;
}

} // anonymous namespace

struct netent *net_by_name(const char *name, NetStorage &storage) {
  if (name == nullptr)
    return nullptr;
  auto status = nss::lookup(
      nss::NETWORKS, "getnetbyname_r",
      [&] {
        return file_net_by_name(name, storage) != nullptr ? nss::SUCCESS
                                                          : nss::NOTFOUND;
      },
      [] { return nss::UNAVAIL; },
      [&](void *fn) {
        using Fn = int (*)(const char *, struct netent *, char *, size_t, int *,
                           int *);
        int error = 0;
        int host_error = 0;
        storage.entry.n_name = nullptr;
        return offered(static_cast<nss::Status>(reinterpret_cast<Fn>(fn)(
                           name, &storage.entry, storage.pool,
                           NetStorage::POOL_SIZE, &error, &host_error)),
                       storage);
      });
  return status == nss::SUCCESS ? &storage.entry : nullptr;
}

struct netent *net_by_addr(uint32_t net, int type, NetStorage &storage) {
  auto status = nss::lookup(
      nss::NETWORKS, "getnetbyaddr_r",
      [&] {
        return file_net_by_addr(net, type, storage) != nullptr ? nss::SUCCESS
                                                               : nss::NOTFOUND;
      },
      [] { return nss::UNAVAIL; },
      [&](void *fn) {
        using Fn = int (*)(uint32_t, int, struct netent *, char *, size_t,
                           int *, int *);
        int error = 0;
        int host_error = 0;
        storage.entry.n_name = nullptr;
        return offered(static_cast<nss::Status>(reinterpret_cast<Fn>(fn)(
                           net, type, &storage.entry, storage.pool,
                           NetStorage::POOL_SIZE, &error, &host_error)),
                       storage);
      });
  return status == nss::SUCCESS ? &storage.entry : nullptr;
}

void rewind_networks(bool stay_open) {
  nss::reset(cursor, "endnetent");
  enumeration.rewind(NETWORKS_PATH, stay_open);
}

void stop_networks() {
  nss::reset(cursor, "endnetent");
  enumeration.stop();
}

struct netent *next_network(NetStorage &storage) {
  auto status = nss::next_entry(
      cursor, nss::NETWORKS, "setnetent", "getnetent_r", "endnetent",
      [&] {
        char line[512];
        while (enumeration.next(NETWORKS_PATH, line, sizeof(line)))
          if (parse(line, storage))
            return nss::SUCCESS;
        return nss::NOTFOUND;
      },
      [&](void *fn, bool is_control) {
        if (is_control)
          return nss::call_control(fn);
        using Fn = int (*)(struct netent *, char *, size_t, int *, int *);
        int error = 0;
        int host_error = 0;
        storage.entry.n_name = nullptr;
        return offered(static_cast<nss::Status>(reinterpret_cast<Fn>(fn)(
                           &storage.entry, storage.pool, NetStorage::POOL_SIZE,
                           &error, &host_error)),
                       storage);
      });
  return status == nss::SUCCESS ? &storage.entry : nullptr;
}

} // namespace resolv
} // namespace LIBC_NAMESPACE_DECL
