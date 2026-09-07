//===-- Looking a service up by name --------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/netdb/resolv/services.h"

#include "hdr/types/struct_servent.h"
#include "src/__support/CPP/string_view.h"
#include "src/__support/macros/attributes.h"
#include "src/__support/macros/config.h"
#include "src/__support/str_to_integer.h"
#include "src/netdb/resolv/file_lines.h"
#include "src/string/memory_utils/inline_memcpy.h"

namespace LIBC_NAMESPACE_DECL {
namespace resolv {

namespace {

constexpr const char *SERVICES_PATH = "/etc/services";

} // anonymous namespace

bool port_for_service(const char *name, const char *protocol, uint16_t &port,
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

namespace {

LIBC_CONSTINIT LIBC_THREAD_LOCAL FileLines service_enumeration;
LIBC_CONSTINIT LIBC_THREAD_LOCAL bool services_keep_open = false;

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

} // anonymous namespace

struct servent *serv_by_name(const char *name, const char *protocol,
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

struct servent *serv_by_port(int port, const char *protocol,
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

void rewind_services(bool stay_open) {
  services_keep_open = stay_open;
  service_enumeration.open(SERVICES_PATH);
}

void stop_services() {
  services_keep_open = false;
  service_enumeration.close_file();
}

struct servent *next_service(ServStorage &storage) {
  if (!service_enumeration.is_open() &&
      !service_enumeration.open(SERVICES_PATH))
    return nullptr;
  char line[512];
  while (service_enumeration.next_line(line, sizeof(line)))
    if (parse_service(line, storage))
      return &storage.entry;
  if (!services_keep_open)
    service_enumeration.close_file();
  return nullptr;
}

} // namespace resolv
} // namespace LIBC_NAMESPACE_DECL
