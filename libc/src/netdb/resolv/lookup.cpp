//===-- Turning a name into addresses -------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/netdb/resolv/lookup.h"

#include "hdr/errno_macros.h"
#include "hdr/fcntl_macros.h"
#include "hdr/netdb_macros.h"
#include "hdr/sys_socket_macros.h"
#include "hdr/types/struct_pollfd.h"
#include "hdr/types/struct_sockaddr_in.h"
#include "hdr/types/struct_sockaddr_in6.h"
#include "include/llvm-libc-macros/linux/poll-macros.h"
#include "src/__support/CPP/string_view.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/close.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/getrandom.h"
#include "src/__support/ctype_utils.h"
#include "src/__support/macros/config.h"
#include "src/arpa/inet/inet_pton.h"
#include "src/netdb/resolv/dns_message.h"
#include "src/netdb/resolv/resolv_conf.h"
#include "src/poll/poll.h"
#include "src/string/memory_utils/inline_memcpy.h"
#include "src/string/string_utils.h"
#include "src/sys/socket/recvfrom.h"
#include "src/sys/socket/sendto.h"
#include "src/sys/socket/socket.h"
#include "src/unistd/close.h"

namespace LIBC_NAMESPACE_DECL {
namespace resolv {

namespace {

// Whether two names are the same, which they are without regard to case.
bool same_name(const char *a, const char *b) {
  for (;; ++a, ++b) {
    if (internal::tolower(*a) != internal::tolower(*b))
      return false;
    if (*a == '\0')
      return true;
  }
}

// Adds one address to the answer, if there is room for it.
void add(Address *out, size_t capacity, size_t &count, int family,
         const unsigned char *bytes, size_t size) {
  if (count >= capacity)
    return;
  Address &entry = out[count];
  entry.family = family;
  inline_memcpy(entry.bytes, bytes, size);
  entry.scope = 0;
  ++count;
}

// Keeps the name an answer really came under, but only the first one: what a
// caller is told is the name the whole lookup ended up at.
void remember_canonical(char *canonical, const char *name) {
  if (canonical == nullptr || canonical[0] != '\0' || name == nullptr)
    return;
  const size_t length = internal::string_length(name);
  if (length > MAX_NAME)
    return;
  inline_memcpy(canonical, name, length);
  canonical[length] = '\0';
}

// The name written as an address, which needs no lookup at all.
int from_numeric(const char *name, int family, Address *out, size_t capacity,
                 size_t &count) {
  unsigned char bytes[16];
  if (family == AF_UNSPEC || family == AF_INET) {
    if (LIBC_NAMESPACE::inet_pton(AF_INET, name, bytes) == 1) {
      add(out, capacity, count, AF_INET, bytes, 4);
      return 1;
    }
  }
  if (family == AF_UNSPEC || family == AF_INET6) {
    // A version six address may name the interface it is on after a percent
    // sign, which is not part of the address.
    char plain[64];
    const char *percent = nullptr;
    for (const char *p = name; *p != '\0'; ++p)
      if (*p == '%') {
        percent = p;
        break;
      }
    const char *text = name;
    if (percent != nullptr) {
      const size_t length = static_cast<size_t>(percent - name);
      if (length >= sizeof(plain))
        return 0;
      inline_memcpy(plain, name, length);
      plain[length] = '\0';
      text = plain;
    }
    if (LIBC_NAMESPACE::inet_pton(AF_INET6, text, bytes) == 1) {
      add(out, capacity, count, AF_INET6, bytes, 16);
      return 1;
    }
  }
  return 0;
}

// The names and addresses an administrator wrote down, which are looked at
// before anything is asked of a name server.
int from_hosts(const char *name, int family, Address *out, size_t capacity,
               size_t &count, char *canonical) {
  HostsFile hosts;
  if (!hosts.open())
    return 0;

  const size_t before = count;
  char line[512];
  while (hosts.next_line(line, sizeof(line))) {
    // An address, then the names it goes by, then whatever a comment says.
    cpp::string_view rest(line);
    cpp::string_view address = hosts.take_field(rest);
    if (address.empty())
      continue;

    bool matched = false;
    cpp::string_view official_name;
    for (cpp::string_view field = hosts.take_field(rest); !field.empty();
         field = hosts.take_field(rest)) {
      if (official_name.empty())
        official_name = field;
      char candidate[MAX_NAME + 1];
      if (field.size() > MAX_NAME)
        continue;
      inline_memcpy(candidate, field.data(), field.size());
      candidate[field.size()] = '\0';
      if (same_name(candidate, name)) {
        matched = true;
        break;
      }
    }
    if (!matched)
      continue;

    char text[64];
    if (address.size() >= sizeof(text))
      continue;
    inline_memcpy(text, address.data(), address.size());
    text[address.size()] = '\0';

    char official[MAX_NAME + 1];
    official[0] = '\0';
    if (!official_name.empty() && official_name.size() <= MAX_NAME) {
      inline_memcpy(official, official_name.data(), official_name.size());
      official[official_name.size()] = '\0';
    }

    unsigned char bytes[16];
    const size_t before_this_line = count;
    if ((family == AF_UNSPEC || family == AF_INET) &&
        LIBC_NAMESPACE::inet_pton(AF_INET, text, bytes) == 1)
      add(out, capacity, count, AF_INET, bytes, 4);
    else if ((family == AF_UNSPEC || family == AF_INET6) &&
             LIBC_NAMESPACE::inet_pton(AF_INET6, text, bytes) == 1)
      add(out, capacity, count, AF_INET6, bytes, 16);
    if (count != before_this_line)
      remember_canonical(canonical, official);
  }
  return static_cast<int>(count - before);
}

// What one exchange with a name server needs to know.
struct Query {
  unsigned char message[MAX_MESSAGE];
  size_t length;
  uint16_t id;
  uint16_t type;
  bool answered;
};

// Asks every server in turn until one answers, and calls `callback` with each
// answer it gives. Returns what the server said.
template <typename F>
Status ask(const ResolvConf &conf, Query &query, F callback) {
  for (size_t attempt = 0; attempt < conf.attempts; ++attempt) {
    for (size_t i = 0; i < conf.server_count; ++i) {
      const Nameserver &server = conf.servers[i];
      int fd =
          LIBC_NAMESPACE::socket(server.family, SOCK_DGRAM | SOCK_CLOEXEC, 0);
      if (fd < 0)
        continue;

      ssize_t sent;
      if (server.family == AF_INET) {
        struct sockaddr_in to = {};
        to.sin_family = AF_INET;
        to.sin_port = server.port;
        inline_memcpy(&to.sin_addr, server.bytes, 4);
        sent = LIBC_NAMESPACE::sendto(fd, query.message, query.length, 0,
                                      reinterpret_cast<struct sockaddr *>(&to),
                                      sizeof(to));
      } else {
        struct sockaddr_in6 to = {};
        to.sin6_family = AF_INET6;
        to.sin6_port = server.port;
        inline_memcpy(&to.sin6_addr, server.bytes, 16);
        sent = LIBC_NAMESPACE::sendto(fd, query.message, query.length, 0,
                                      reinterpret_cast<struct sockaddr *>(&to),
                                      sizeof(to));
      }
      if (sent < 0) {
        LIBC_NAMESPACE::close(fd);
        continue;
      }

      struct pollfd waiting = {fd, POLLIN, 0};
      const int ready = LIBC_NAMESPACE::poll(
          &waiting, 1, static_cast<int>(conf.timeout_seconds) * 1000);
      if (ready <= 0) {
        LIBC_NAMESPACE::close(fd);
        continue;
      }

      unsigned char reply[MAX_MESSAGE];
      const ssize_t taken = LIBC_NAMESPACE::recvfrom(fd, reply, sizeof(reply),
                                                     0, nullptr, nullptr);
      LIBC_NAMESPACE::close(fd);
      if (taken <= 0)
        continue;

      const Status status = for_each_answer(
          reply, static_cast<size_t>(taken), query.id,
          [&](const Record &record) { callback(reply, taken, record); });
      if (status != Status::Failed)
        return status;
    }
  }
  return Status::Failed;
}

// Asks the name servers for one kind of address.
Status from_dns_one(const ResolvConf &conf, const char *name, uint16_t type,
                    int family, size_t size, Address *out, size_t capacity,
                    size_t &count, char *canonical) {
  uint16_t id = 0;
  if (!linux_syscalls::getrandom(&id, sizeof(id), 0).has_value())
    return Status::Failed;

  Query query;
  query.id = id;
  query.type = type;
  query.length = build_query(name, type, id, query.message, MAX_MESSAGE);
  if (query.length == 0)
    return Status::Failed;

  return ask(
      conf, query,
      [&](const unsigned char *message, ssize_t length, const Record &record) {
        // A server may answer with the name the one asked about
        // really stands for, which is the name to report back.
        if (record.type == TYPE_CNAME) {
          char real[MAX_NAME + 1];
          real[0] = '\0';
          if (read_name(message, static_cast<size_t>(length),
                        static_cast<size_t>(record.data - message), real,
                        sizeof(real)) != 0)
            remember_canonical(canonical, real);
          return;
        }
        if (record.type != type || record.length != size)
          return;
        add(out, capacity, count, family, record.data, size);
      });
}

} // anonymous namespace

int lookup_name(const char *name, int family, int flags, Address *out,
                size_t capacity, char *canonical) {
  if (out == nullptr || capacity == 0)
    return EAI_SYSTEM;
  if (family != AF_UNSPEC && family != AF_INET && family != AF_INET6)
    return EAI_FAMILY;

  size_t count = 0;
  if (canonical != nullptr)
    canonical[0] = '\0';

  // A name with no name in it means the machine this is running on, which
  // getaddrinfo's caller fills in for itself.
  if (name == nullptr || name[0] == '\0')
    return EAI_NONAME;

  if (from_numeric(name, family, out, capacity, count) != 0)
    return static_cast<int>(count);
  // Where the caller said the name is written as an address, nothing else is
  // tried: there is nothing to look up.
  if ((flags & AI_NUMERICHOST) != 0)
    return EAI_NONAME;

  if (from_hosts(name, family, out, capacity, count, canonical) != 0) {
    prefer_newer_addresses(out, count);
    return static_cast<int>(count);
  }

  ResolvConf conf;
  if (!conf.read())
    return EAI_NONAME;

  Status status = Status::NoName;
  // The version six addresses come first, which is the order a machine with
  // both should try them in.
  if (family == AF_UNSPEC || family == AF_INET6) {
    const Status one = from_dns_one(conf, name, TYPE_AAAA, AF_INET6, 16, out,
                                    capacity, count, canonical);
    if (one == Status::Ok)
      status = Status::Ok;
    else if (one == Status::Failed && status != Status::Ok)
      status = Status::Failed;
  }
  if (family == AF_UNSPEC || family == AF_INET) {
    const Status one = from_dns_one(conf, name, TYPE_A, AF_INET, 4, out,
                                    capacity, count, canonical);
    if (one == Status::Ok)
      status = Status::Ok;
    else if (one == Status::Failed && status != Status::Ok)
      status = Status::Failed;
  }

  if (count != 0) {
    prefer_newer_addresses(out, count);
    return static_cast<int>(count);
  }
  if (status == Status::Failed)
    return EAI_AGAIN;
  return EAI_NONAME;
}

void prefer_newer_addresses(Address *out, size_t count) {
  // A machine with both kinds tries the newer one first, which is what the
  // rules for choosing between them come down to in the ordinary case. The
  // order within each kind is left as it was, since that is the order the
  // answer gave and it carries what the server wanted.
  size_t next = 0;
  for (size_t pass = 0; pass < 2; ++pass) {
    const int wanted = pass == 0 ? AF_INET6 : AF_INET;
    for (size_t i = next; i < count; ++i) {
      if (out[i].family != wanted)
        continue;
      if (i != next) {
        const Address moved = out[i];
        for (size_t j = i; j > next; --j)
          out[j] = out[j - 1];
        out[next] = moved;
      }
      ++next;
    }
  }
}

} // namespace resolv
} // namespace LIBC_NAMESPACE_DECL
