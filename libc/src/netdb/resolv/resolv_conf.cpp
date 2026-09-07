//===-- What the machine was told about name servers ----------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/netdb/resolv/resolv_conf.h"

#include "hdr/fcntl_macros.h"
#include "hdr/sys_socket_macros.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/open.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/read.h"
#include "src/__support/ctype_utils.h"
#include "src/__support/macros/config.h"
#include "src/__support/str_to_integer.h"
#include "src/arpa/inet/inet_pton.h"
#include "src/net/if_nametoindex.h"
#include "src/string/memory_utils/inline_memcpy.h"
#include "src/unistd/close.h"

namespace LIBC_NAMESPACE_DECL {
namespace resolv {

namespace {

constexpr const char *RESOLV_CONF_PATH = "/etc/resolv.conf";
constexpr const char *HOSTS_PATH = "/etc/hosts";
// The port a name server listens on, in the order the wire uses.
constexpr uint16_t DNS_PORT = 0x3500;

// Takes the next run of non blank characters off the front of `rest`.
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

// Reads a whole small file into `buffer`, returning how much there was.
size_t read_file(const char *path, char *buffer, size_t capacity) {
  auto fd = linux_syscalls::open(path, O_RDONLY | O_CLOEXEC, 0);
  if (!fd.has_value())
    return 0;
  size_t filled = 0;
  while (filled < capacity) {
    auto taken =
        linux_syscalls::read(fd.value(), buffer + filled, capacity - filled);
    if (!taken.has_value() || taken.value() == 0)
      break;
    filled += static_cast<size_t>(taken.value());
  }
  LIBC_NAMESPACE::close(fd.value());
  return filled;
}

} // anonymous namespace

bool ResolvConf::read() {
  // The file is a few lines. Anything past this is not something anybody
  // meant, and reading it would be reading somebody else's memory.
  char contents[4096];
  const size_t filled = read_file(RESOLV_CONF_PATH, contents, sizeof(contents));

  cpp::string_view rest(contents, filled);
  while (!rest.empty()) {
    size_t end = 0;
    while (end < rest.size() && rest[end] != '\n')
      ++end;
    cpp::string_view line = rest.substr(0, end);
    rest = end < rest.size() ? rest.substr(end + 1) : cpp::string_view();

    // Anything after a comment marker is for people to read.
    for (size_t i = 0; i < line.size(); ++i)
      if (line[i] == '#' || line[i] == ';') {
        line = line.substr(0, i);
        break;
      }

    cpp::string_view keyword = next_field(line);
    if (equal(keyword, "nameserver")) {
      if (server_count >= MAX_SERVERS)
        continue;
      cpp::string_view address = next_field(line);
      char text[64];
      if (address.empty() || address.size() >= sizeof(text))
        continue;
      inline_memcpy(text, address.data(), address.size());
      text[address.size()] = '\0';

      // A link local address carries the interface it is on after a percent
      // sign, written either as the name or as the number.
      char zone[64];
      zone[0] = '\0';
      for (size_t i = 0; text[i] != '\0'; ++i)
        if (text[i] == '%') {
          text[i] = '\0';
          size_t j = 0;
          for (; text[i + 1 + j] != '\0' && j + 1 < sizeof(zone); ++j)
            zone[j] = text[i + 1 + j];
          zone[j] = '\0';
          break;
        }

      Nameserver &server = servers[server_count];
      server.port = DNS_PORT;
      server.scope = 0;
      if (LIBC_NAMESPACE::inet_pton(AF_INET, text, server.bytes) == 1) {
        server.family = AF_INET;
        ++server_count;
      } else if (LIBC_NAMESPACE::inet_pton(AF_INET6, text, server.bytes) == 1) {
        server.family = AF_INET6;
        if (zone[0] != '\0') {
          auto number = internal::strtointeger<unsigned int>(zone, 10);
          server.scope = number.has_error() || number.value == 0
                             ? LIBC_NAMESPACE::if_nametoindex(zone)
                             : number.value;
        }
        ++server_count;
      }
      continue;
    }
    // Both name the domains an unqualified name is tried in. The last line
    // of either kind wins, which is what the file has always meant.
    if (equal(keyword, "search") || equal(keyword, "domain")) {
      search_count = 0;
      search_used = 0;
      for (cpp::string_view name = next_field(line); !name.empty();
           name = next_field(line)) {
        if (search_count == MAX_SEARCH)
          break;
        if (search_used + name.size() + 1 > SEARCH_POOL)
          break;
        char *at = search_pool + search_used;
        inline_memcpy(at, name.data(), name.size());
        at[name.size()] = '\0';
        search_used += name.size() + 1;
        search[search_count++] = at;
        // A domain line names one domain, whatever else is on it.
        if (equal(keyword, "domain"))
          break;
      }
      continue;
    }
    if (equal(keyword, "options")) {
      for (cpp::string_view option = next_field(line); !option.empty();
           option = next_field(line)) {
        // Both of these are written as a word, a colon and a number.
        size_t colon = 0;
        while (colon < option.size() && option[colon] != ':')
          ++colon;
        if (colon == option.size())
          continue;
        cpp::string_view word = option.substr(0, colon);
        char number[16];
        cpp::string_view digits = option.substr(colon + 1);
        if (digits.empty() || digits.size() >= sizeof(number))
          continue;
        inline_memcpy(number, digits.data(), digits.size());
        number[digits.size()] = '\0';
        auto value = internal::strtointeger<int>(number, 10);
        if (value.value < 0)
          continue;
        // Waiting no time at all or asking no times at all would mean never
        // getting an answer, so those are left as they were. Trying a name
        // in no search domain first is a real thing to ask for.
        if (equal(word, "timeout") && value.value > 0)
          timeout_seconds = static_cast<size_t>(value.value);
        else if (equal(word, "attempts") && value.value > 0)
          attempts = static_cast<size_t>(value.value);
        else if (equal(word, "ndots"))
          ndots = value.value > 15 ? 15 : static_cast<size_t>(value.value);
      }
    }
  }

  // Where the file named nobody, the machine itself is asked, which is what
  // every other libc falls back to.
  if (server_count == 0) {
    Nameserver &server = servers[0];
    server.family = AF_INET;
    server.bytes[0] = 127;
    server.bytes[1] = 0;
    server.bytes[2] = 0;
    server.bytes[3] = 1;
    server.port = DNS_PORT;
    server_count = 1;
  }
  return true;
}

HostsFile::~HostsFile() {
  if (fd_ >= 0)
    LIBC_NAMESPACE::close(fd_);
}

bool HostsFile::open() {
  auto fd = linux_syscalls::open(HOSTS_PATH, O_RDONLY | O_CLOEXEC, 0);
  if (!fd.has_value())
    return false;
  fd_ = fd.value();
  return true;
}

bool HostsFile::fill() {
  if (ended_)
    return false;
  auto taken = linux_syscalls::read(fd_, buffer_, sizeof(buffer_));
  if (!taken.has_value() || taken.value() == 0) {
    ended_ = true;
    return false;
  }
  filled_ = static_cast<size_t>(taken.value());
  at_ = 0;
  return true;
}

bool HostsFile::next_line(char *line, size_t capacity) {
  for (;;) {
    size_t written = 0;
    bool any = false;
    for (;;) {
      if (at_ >= filled_ && !fill())
        break;
      any = true;
      const char c = buffer_[at_++];
      if (c == '\n')
        break;
      // A line longer than the buffer is truncated rather than run past.
      if (written + 1 < capacity)
        line[written++] = c;
    }
    if (!any)
      return false;
    line[written] = '\0';

    // Anything after a comment marker is for people to read.
    for (size_t i = 0; i < written; ++i)
      if (line[i] == '#') {
        line[i] = '\0';
        written = i;
        break;
      }
    for (size_t i = 0; i < written; ++i)
      if (!internal::isspace(line[i]))
        return true;
    // A line with nothing on it is skipped rather than handed back.
  }
}

cpp::string_view HostsFile::take_field(cpp::string_view &rest) {
  return next_field(rest);
}

} // namespace resolv
} // namespace LIBC_NAMESPACE_DECL
