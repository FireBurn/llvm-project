//===-- Looking a service up by name --------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/netdb/resolv/services.h"

#include "src/__support/CPP/string_view.h"
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

} // namespace resolv
} // namespace LIBC_NAMESPACE_DECL
