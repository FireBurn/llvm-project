//===-- Looking a service up by name --------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/netdb/resolv/services.h"

#include "hdr/fcntl_macros.h"
#include "src/__support/CPP/string_view.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/open.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/read.h"
#include "src/__support/ctype_utils.h"
#include "src/__support/macros/config.h"
#include "src/__support/str_to_integer.h"
#include "src/string/memory_utils/inline_memcpy.h"
#include "src/unistd/close.h"

namespace LIBC_NAMESPACE_DECL {
namespace resolv {

namespace {

constexpr const char *SERVICES_PATH = "/etc/services";

cpp::string_view next_field(cpp::string_view &rest) {
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

bool equal(cpp::string_view field, const char *word) {
  size_t i = 0;
  for (; word[i] != '\0'; ++i)
    if (i >= field.size() || field[i] != word[i])
      return false;
  return i == field.size();
}

// Reads one line at a time from a file, keeping only a buffer's worth.
class Reader {
public:
  ~Reader() {
    if (fd_ >= 0)
      LIBC_NAMESPACE::close(fd_);
  }

  bool open(const char *path) {
    auto fd = linux_syscalls::open(path, O_RDONLY | O_CLOEXEC, 0);
    if (!fd.has_value())
      return false;
    fd_ = fd.value();
    return true;
  }

  bool next_line(char *line, size_t capacity) {
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
  char buffer_[1024];
  size_t filled_ = 0;
  size_t at_ = 0;
  bool ended_ = false;
};

} // anonymous namespace

bool port_for_service(const char *name, const char *protocol, uint16_t &port,
                      char *found_protocol, size_t found_capacity) {
  if (name == nullptr)
    return false;
  if (found_protocol != nullptr && found_capacity != 0)
    found_protocol[0] = '\0';

  Reader reader;
  if (!reader.open(SERVICES_PATH))
    return false;

  char line[512];
  while (reader.next_line(line, sizeof(line))) {
    // Anything after a comment marker is for people to read.
    for (size_t i = 0; line[i] != '\0'; ++i)
      if (line[i] == '#') {
        line[i] = '\0';
        break;
      }

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
    return true;
  }
  return false;
}

} // namespace resolv
} // namespace LIBC_NAMESPACE_DECL
