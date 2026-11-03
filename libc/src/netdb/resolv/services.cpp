//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Looking a service up by name
///
//===----------------------------------------------------------------------===//

#include "src/netdb/resolv/services.h"

#include "hdr/errno_macros.h"
#include "hdr/types/struct_servent.h"
#include "src/__support/CPP/string_view.h"
#include "src/__support/macros/attributes.h"
#include "src/__support/macros/config.h"
#include "src/__support/nss/dispatch.h"
#include "src/__support/nss/lookup.h"
#include "src/__support/str_to_integer.h"
#include "src/netdb/resolv/file_lines.h"
#include "src/string/memory_utils/inline_memcpy.h"

namespace LIBC_NAMESPACE_DECL {
namespace resolv {

namespace {

constexpr const char *SERVICES_PATH = "/etc/services";

} // anonymous namespace

namespace {

bool port_from_file(const char *name, const char *protocol, uint16_t &port,
                    char *found_protocol, size_t found_capacity) {
  if (name == nullptr)
    return false;
  if (found_protocol != nullptr && found_capacity != 0)
    found_protocol[0] = '\0';

  FileLines reader;
  if (!reader.open(SERVICES_PATH))
    return false;
  bool found = false;

  char line[512];
  while (!found && reader.next_line(line, sizeof(line))) {
    drop_comment(line);

    // A name, then the port and the protocol joined by a slash, then any
    // other names the service goes by.
    cpp::string_view rest(line);
    cpp::string_view official = next_field(rest);
    if (official.empty())
      continue;
    cpp::string_view port_and_protocol = next_field(rest);
    if (port_and_protocol.empty())
      continue;

    size_t slash = 0;
    while (slash < port_and_protocol.size() && port_and_protocol[slash] != '/')
      ++slash;
    if (slash == port_and_protocol.size())
      continue;
    cpp::string_view digits = port_and_protocol.substr(0, slash);
    cpp::string_view named = port_and_protocol.substr(slash + 1);

    if (protocol != nullptr && !equal(named, protocol))
      continue;

    bool matched = equal(official, name);
    if (!matched)
      for (cpp::string_view other = next_field(rest); !other.empty();
           other = next_field(rest))
        if (equal(other, name)) {
          matched = true;
          break;
        }
    if (!matched)
      continue;

    char number[16];
    if (digits.empty() || digits.size() >= sizeof(number))
      continue;
    inline_memcpy(number, digits.data(), digits.size());
    number[digits.size()] = '\0';
    auto value = internal::strtointeger<long>(number, 10);
    if (value.value < 0 || value.value > 65535)
      continue;

    const uint16_t host_order = static_cast<uint16_t>(value.value);
    port = static_cast<uint16_t>((host_order << 8) | (host_order >> 8));
    if (found_protocol != nullptr && named.size() < found_capacity) {
      inline_memcpy(found_protocol, named.data(), named.size());
      found_protocol[named.size()] = '\0';
    }
    found = true;
  }
  reader.close_file();
  return found;
}

// Where the enumeration has got to. setservent asks for the file to be held
// open across lookups, which is what the enumeration records.
LIBC_CONSTINIT LIBC_THREAD_LOCAL Enumeration service_enumeration;
LIBC_THREAD_LOCAL nss::Cursor service_cursor;

char *keep_service(cpp::string_view field, ServStorage &storage) {
  if (storage.used + field.size() + 1 > ServStorage::POOL_SIZE)
    return nullptr;
  char *at = storage.pool + storage.used;
  inline_memcpy(at, field.data(), field.size());
  at[field.size()] = '\0';
  storage.used += field.size() + 1;
  return at;
}

// A line names the service, then the port and the protocol joined by a
// slash, then whatever else it goes by.
bool parse_service(char *line, ServStorage &storage) {
  drop_comment(line);

  cpp::string_view rest(line);
  cpp::string_view official = next_field(rest);
  if (official.empty())
    return false;
  cpp::string_view port_and_protocol = next_field(rest);
  if (port_and_protocol.empty())
    return false;

  size_t slash = 0;
  while (slash < port_and_protocol.size() && port_and_protocol[slash] != '/')
    ++slash;
  if (slash == port_and_protocol.size())
    return false;
  cpp::string_view digits = port_and_protocol.substr(0, slash);
  cpp::string_view named = port_and_protocol.substr(slash + 1);

  char number[16];
  if (digits.empty() || digits.size() >= sizeof(number))
    return false;
  inline_memcpy(number, digits.data(), digits.size());
  number[digits.size()] = '\0';
  auto value = internal::strtointeger<long>(number, 10);
  if (value.value < 0 || value.value > 65535)
    return false;

  storage.used = 0;
  char *name = keep_service(official, storage);
  char *protocol = keep_service(named, storage);
  if (name == nullptr || protocol == nullptr)
    return false;

  size_t count = 0;
  for (cpp::string_view other = next_field(rest); !other.empty();
       other = next_field(rest)) {
    if (count == ServStorage::MAX_ALIASES)
      break;
    char *kept = keep_service(other, storage);
    if (kept == nullptr)
      break;
    storage.aliases[count++] = kept;
  }
  storage.aliases[count] = nullptr;

  const uint16_t host_order = static_cast<uint16_t>(value.value);
  storage.entry.s_name = name;
  storage.entry.s_aliases = storage.aliases;
  // The port is reported in the order the wire uses, as it always has been.
  storage.entry.s_port =
      static_cast<int>((host_order << 8) | (host_order >> 8));
  storage.entry.s_proto = protocol;
  return true;
}

bool service_names_match(const ServStorage &storage, const char *name) {
  if (equal(cpp::string_view(storage.entry.s_name), name))
    return true;
  for (size_t i = 0; storage.aliases[i] != nullptr; ++i)
    if (equal(cpp::string_view(storage.aliases[i]), name))
      return true;
  return false;
}

bool protocol_matches(const ServStorage &storage, const char *protocol) {
  return protocol == nullptr ||
         equal(cpp::string_view(storage.entry.s_proto), protocol);
}

struct servent *file_serv_by_name(const char *name, const char *protocol,
                                  ServStorage &storage) {
  if (name == nullptr)
    return nullptr;
  FileLines reader;
  if (!reader.open(SERVICES_PATH))
    return nullptr;
  struct servent *found = nullptr;
  char line[512];
  while (found == nullptr && reader.next_line(line, sizeof(line)))
    if (parse_service(line, storage) && protocol_matches(storage, protocol) &&
        service_names_match(storage, name))
      found = &storage.entry;
  reader.close_file();
  return found;
}

struct servent *file_serv_by_port(int port, const char *protocol,
                                  ServStorage &storage) {
  FileLines reader;
  if (!reader.open(SERVICES_PATH))
    return nullptr;
  struct servent *found = nullptr;
  char line[512];
  while (found == nullptr && reader.next_line(line, sizeof(line)))
    if (parse_service(line, storage) && protocol_matches(storage, protocol) &&
        storage.entry.s_port == port)
      found = &storage.entry;
  reader.close_file();
  return found;
}

// What a module put in the storage, as the answer.
nss::Status served(nss::Status status, ServStorage &storage, int error) {
  if (status != nss::SUCCESS)
    return status == nss::TRYAGAIN && error != ERANGE ? status : nss::NOTFOUND;
  return storage.entry.s_name != nullptr ? nss::SUCCESS : nss::NOTFOUND;
}

} // anonymous namespace

struct servent *serv_by_name(const char *name, const char *protocol,
                             ServStorage &storage) {
  if (name == nullptr)
    return nullptr;
  auto status = nss::lookup(
      nss::SERVICES, "getservbyname_r",
      [&] {
        return file_serv_by_name(name, protocol, storage) != nullptr
                   ? nss::SUCCESS
                   : nss::NOTFOUND;
      },
      [] { return nss::UNAVAIL; },
      [&](void *fn) {
        using Fn = int (*)(const char *, const char *, struct servent *, char *,
                           size_t, int *);
        int error = 0;
        storage.entry.s_name = nullptr;
        return served(static_cast<nss::Status>(reinterpret_cast<Fn>(fn)(
                          name, protocol, &storage.entry, storage.pool,
                          ServStorage::POOL_SIZE, &error)),
                      storage, error);
      });
  return status == nss::SUCCESS ? &storage.entry : nullptr;
}

struct servent *serv_by_port(int port, const char *protocol,
                             ServStorage &storage) {
  auto status = nss::lookup(
      nss::SERVICES, "getservbyport_r",
      [&] {
        return file_serv_by_port(port, protocol, storage) != nullptr
                   ? nss::SUCCESS
                   : nss::NOTFOUND;
      },
      [] { return nss::UNAVAIL; },
      [&](void *fn) {
        using Fn =
            int (*)(int, const char *, struct servent *, char *, size_t, int *);
        int error = 0;
        storage.entry.s_name = nullptr;
        return served(static_cast<nss::Status>(reinterpret_cast<Fn>(fn)(
                          port, protocol, &storage.entry, storage.pool,
                          ServStorage::POOL_SIZE, &error)),
                      storage, error);
      });
  return status == nss::SUCCESS ? &storage.entry : nullptr;
}

bool port_for_service(const char *name, const char *protocol, uint16_t &port,
                      char *found_protocol, size_t found_capacity) {
  if (name == nullptr)
    return false;
  if (found_protocol != nullptr && found_capacity != 0)
    found_protocol[0] = '\0';
  // Asks the sources the way serv_by_name does, but reads the file without
  // the limits of the storage a servent keeps its strings in.
  ServStorage storage;
  auto status = nss::lookup(
      nss::SERVICES, "getservbyname_r",
      [&] {
        return port_from_file(name, protocol, port, found_protocol,
                              found_capacity)
                   ? nss::SUCCESS
                   : nss::NOTFOUND;
      },
      [] { return nss::UNAVAIL; },
      [&](void *fn) {
        using Fn = int (*)(const char *, const char *, struct servent *, char *,
                           size_t, int *);
        int error = 0;
        storage.entry.s_name = nullptr;
        auto result = served(static_cast<nss::Status>(reinterpret_cast<Fn>(fn)(
                                 name, protocol, &storage.entry, storage.pool,
                                 ServStorage::POOL_SIZE, &error)),
                             storage, error);
        if (result == nss::SUCCESS) {
          port = static_cast<uint16_t>(storage.entry.s_port);
          const char *named = storage.entry.s_proto;
          if (found_protocol != nullptr && named != nullptr) {
            size_t length = 0;
            while (named[length] != '\0')
              ++length;
            if (length < found_capacity) {
              inline_memcpy(found_protocol, named, length + 1);
            }
          }
        }
        return result;
      });
  return status == nss::SUCCESS;
}

void rewind_services(bool stay_open) {
  nss::end_entries(service_cursor, "endservent",
                   [](void *fn, bool) { return nss::call_control(fn); });
  service_enumeration.rewind(SERVICES_PATH, stay_open);
}

void stop_services() {
  nss::end_entries(service_cursor, "endservent",
                   [](void *fn, bool) { return nss::call_control(fn); });
  service_enumeration.stop();
}

struct servent *next_service(ServStorage &storage) {
  auto status = nss::next_entry(
      service_cursor, nss::SERVICES, "setservent", "getservent_r", "endservent",
      [&] {
        char line[512];
        while (service_enumeration.next(SERVICES_PATH, line, sizeof(line)))
          if (parse_service(line, storage))
            return nss::SUCCESS;
        return nss::NOTFOUND;
      },
      [&](void *fn, bool is_control) {
        if (is_control)
          return nss::call_control(fn);
        using Fn = int (*)(struct servent *, char *, size_t, int *);
        int error = 0;
        storage.entry.s_name = nullptr;
        auto result = static_cast<nss::Status>(reinterpret_cast<Fn>(fn)(
            &storage.entry, storage.pool, ServStorage::POOL_SIZE, &error));
        return served(result, storage, error);
      });
  return status == nss::SUCCESS ? &storage.entry : nullptr;
}

} // namespace resolv
} // namespace LIBC_NAMESPACE_DECL
