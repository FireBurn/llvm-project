//===-- What the machine was told about name servers ------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_NETDB_RESOLV_RESOLV_CONF_H
#define LLVM_LIBC_SRC_NETDB_RESOLV_RESOLV_CONF_H

#include "hdr/stdint_proxy.h"
#include "hdr/types/size_t.h"
#include "src/__support/CPP/string_view.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {
namespace resolv {

// How many servers are worth keeping. Three is what the file format has
// always allowed and what every other libc reads.
constexpr size_t MAX_SERVERS = 3;

struct Nameserver {
  int family;
  unsigned char bytes[16];
  // Already in the order the wire uses.
  uint16_t port;
  // Which interface a link local address is on. A server written as
  // fe80::1%eth0 can only be reached through the one it names, and cannot be
  // reached at all without it.
  uint32_t scope;
};

// How many search domains are worth keeping, and how much room their names
// take between them. Six is what the file format allows and what every other
// libc reads.
constexpr size_t MAX_SEARCH = 6;
constexpr size_t SEARCH_POOL = 256;

// What /etc/resolv.conf says, or the defaults where it says nothing.
struct ResolvConf {
  Nameserver servers[MAX_SERVERS];
  size_t server_count = 0;
  size_t timeout_seconds = 5;
  size_t attempts = 2;

  // The domains a name with too few dots in it is tried in, in order.
  char *search[MAX_SEARCH];
  size_t search_count = 0;
  char search_pool[SEARCH_POOL];
  size_t search_used = 0;

  // How many dots a name needs before it is tried as it stands rather than
  // in the search domains first.
  size_t ndots = 1;

  // Reads the file. Returns false if there is nothing to ask.
  bool read();
};

// Reads /etc/hosts a line at a time. The file is small and read once, so this
// keeps no more of it than the line being looked at.
class HostsFile {
public:
  ~HostsFile();

  bool open();

  // Fills `line` with the next line that holds anything, with any comment
  // taken off. Returns false at the end of the file.
  bool next_line(char *line, size_t capacity);

  // Takes the next run of non blank characters off the front of `rest`.
  static cpp::string_view take_field(cpp::string_view &rest);

private:
  int fd_ = -1;
  char buffer_[1024];
  size_t filled_ = 0;
  size_t at_ = 0;
  bool ended_ = false;

  bool fill();
};

} // namespace resolv
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_NETDB_RESOLV_RESOLV_CONF_H
