//===-- Reading the hosts database ----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/netdb/resolv/hosts.h"

#include "hdr/sys_socket_macros.h"
#include "src/__support/CPP/string_view.h"
#include "src/__support/macros/attributes.h"
#include "src/__support/macros/config.h"
#include "src/arpa/inet/inet_pton.h"
#include "src/netdb/resolv/file_lines.h"
#include "src/string/memory_utils/inline_memcpy.h"

namespace LIBC_NAMESPACE_DECL {
namespace resolv {

namespace {

constexpr const char *HOSTS_PATH = "/etc/hosts";

// ::ffff:a.b.c.d, which is how an IPv4 address is written the long way.
bool is_v4_mapped(const unsigned char *bytes) {
  for (size_t i = 0; i < 10; ++i)
    if (bytes[i] != 0)
      return false;
  return bytes[10] == 0xff && bytes[11] == 0xff;
}

// ::1, which names the same machine as 127.0.0.1 does.
bool is_loopback(const unsigned char *bytes) {
  for (size_t i = 0; i < 15; ++i)
    if (bytes[i] != 0)
      return false;
  return bytes[15] == 1;
}

// Where the enumeration has got to. sethostent asks for the file to be held
// open across lookups, which is what the enumeration records.
LIBC_CONSTINIT LIBC_THREAD_LOCAL Enumeration enumeration;

// Copies a field into the storage's pool and hands back a pointer to it, or
// null where there is no room left.
char *keep(cpp::string_view field, HostsStorage &storage) {
  if (storage.used + field.size() + 1 > HostsStorage::POOL_SIZE)
    return nullptr;
  char *at = storage.pool + storage.used;
  inline_memcpy(at, field.data(), field.size());
  at[field.size()] = '\0';
  storage.used += field.size() + 1;
  return at;
}

// Reads one line of the file into `storage`. Returns false where the line
// holds no entry, which a comment, a blank line or a line naming an address
// nobody goes by does not.
bool parse(char *line, HostsStorage &storage) {
  drop_comment(line);

  // An address, then the name it goes by, then any other names.
  cpp::string_view rest(line);
  cpp::string_view address = next_field(rest);
  if (address.empty())
    return false;

  char text[64];
  if (address.size() >= sizeof(text))
    return false;
  inline_memcpy(text, address.data(), address.size());
  text[address.size()] = '\0';

  // Walking the file hands back IPv4 entries and nothing else: the interface
  // gives no way to ask for a family, and everything written against it
  // assumes four octets. An address written the long way is taken where it
  // stands for one of those, and the line is passed over otherwise.
  if (LIBC_NAMESPACE::inet_pton(AF_INET, text, storage.address) != 1) {
    unsigned char wide[16];
    if (LIBC_NAMESPACE::inet_pton(AF_INET6, text, wide) != 1)
      return false;
    if (is_v4_mapped(wide)) {
      for (size_t i = 0; i < 4; ++i)
        storage.address[i] = wide[12 + i];
    } else if (is_loopback(wide)) {
      storage.address[0] = 127;
      storage.address[1] = 0;
      storage.address[2] = 0;
      storage.address[3] = 1;
    } else {
      return false;
    }
  }

  cpp::string_view official = next_field(rest);
  if (official.empty())
    return false;

  storage.used = 0;
  char *name = keep(official, storage);
  if (name == nullptr)
    return false;

  size_t count = 0;
  for (cpp::string_view other = next_field(rest); !other.empty();
       other = next_field(rest)) {
    if (count == HostsStorage::MAX_ALIASES)
      break;
    char *kept = keep(other, storage);
    if (kept == nullptr)
      break;
    storage.aliases[count++] = kept;
  }
  storage.aliases[count] = nullptr;

  storage.address_list[0] = reinterpret_cast<char *>(storage.address);
  storage.address_list[1] = nullptr;

  storage.entry.h_name = name;
  storage.entry.h_aliases = storage.aliases;
  storage.entry.h_addrtype = AF_INET;
  storage.entry.h_length = 4;
  storage.entry.h_addr_list = storage.address_list;
  return true;
}

} // anonymous namespace

void rewind_hosts(bool stay_open) { enumeration.rewind(HOSTS_PATH, stay_open); }

void stop_hosts() { enumeration.stop(); }

struct hostent *next_host(HostsStorage &storage) {
  char line[512];
  while (enumeration.next(HOSTS_PATH, line, sizeof(line)))
    if (parse(line, storage))
      return &storage.entry;
  return nullptr;
}

} // namespace resolv
} // namespace LIBC_NAMESPACE_DECL
