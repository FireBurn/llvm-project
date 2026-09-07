//===-- The resolver's state ----------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/resolv/res_state.h"

#include "hdr/sys_socket_macros.h"
#include "src/__support/common.h"
#include "src/__support/macros/attributes.h"
#include "src/__support/macros/config.h"
#include "src/netdb/resolv/resolv_conf.h"
#include "src/string/memory_utils/inline_memcpy.h"

namespace LIBC_NAMESPACE_DECL {

namespace {

// The names the search list points at have to outlive the call that read
// them, so they are kept beside the state rather than in it.
LIBC_CONSTINIT LIBC_THREAD_LOCAL char search_names[resolv::SEARCH_POOL];

} // anonymous namespace

LLVM_LIBC_FUNCTION(struct __res_state *, __res_state, (void)) {
  static LIBC_THREAD_LOCAL struct __res_state state;
  return &state;
}

namespace internal {

bool res_setup(struct __res_state &state) {
  resolv::ResolvConf conf;
  const bool have_servers = conf.read();

  state.retrans = static_cast<int>(conf.timeout_seconds);
  state.retry = static_cast<int>(conf.attempts);
  state.options = RES_DEFAULT | RES_INIT;
  state.ndots = conf.ndots > 15 ? 15 : static_cast<unsigned>(conf.ndots);
  state.nsort = 0;
  state.pfcode = 0;
  state.res_h_errno = 0;
  state._vcsock = -1;
  state._flags = 0;
  state.id = 0;
  state.defdname[0] = '\0';

  // Every server found is counted, but only an IPv4 one can be described:
  // nsaddr_list has always been an array of sockaddr_in. A server reached
  // over IPv6 is asked all the same and its slot is left blank, which is what
  // a program reading this finds everywhere else too.
  state.nscount = 0;
  for (size_t i = 0; i < conf.server_count && state.nscount < MAXNS; ++i) {
    struct sockaddr_in &to = state.nsaddr_list[state.nscount++];
    to = {};
    if (conf.servers[i].family != AF_INET)
      continue;
    to.sin_family = AF_INET;
    to.sin_port = conf.servers[i].port;
    inline_memcpy(&to.sin_addr, conf.servers[i].bytes, 4);
  }

  size_t used = 0;
  size_t count = 0;
  for (size_t i = 0; i < conf.search_count && count < MAXDNSRCH; ++i) {
    size_t length = 0;
    while (conf.search[i][length] != '\0')
      ++length;
    if (used + length + 1 > sizeof(search_names))
      break;
    char *at = search_names + used;
    inline_memcpy(at, conf.search[i], length + 1);
    used += length + 1;
    state.dnsrch[count++] = at;
    if (count == 1)
      inline_memcpy(state.defdname, at,
                    length + 1 > sizeof(state.defdname) ? sizeof(state.defdname)
                                                        : length + 1);
  }
  state.dnsrch[count] = nullptr;

  return have_servers;
}

bool res_ready(struct __res_state &state) {
  if ((state.options & RES_INIT) != 0)
    return state.nscount > 0 || state.dnsrch[0] != nullptr;
  return res_setup(state);
}

void conf_from_state(const struct __res_state &state, resolv::ResolvConf &out) {
  // Whatever the file said, what is asked is what the state now holds: a
  // program may reach in and change it before asking anything, and that is
  // the whole reason the state is public.
  out.read();
  out.timeout_seconds =
      state.retrans > 0 ? static_cast<size_t>(state.retrans) : 5;
  out.attempts = state.retry > 0 ? static_cast<size_t>(state.retry) : 2;
  out.ndots = state.ndots;

  // A caller may have written its own servers into the state, so those are
  // what get asked. A blank slot stands for a server the state cannot
  // describe, and the one the file named is kept for it.
  bool named_any = false;
  for (int i = 0; i < state.nscount; ++i)
    if (state.nsaddr_list[i].sin_family == AF_INET)
      named_any = true;
  if (!named_any)
    return;

  resolv::ResolvConf from_file = out;
  out.server_count = 0;
  for (int i = 0; i < state.nscount && out.server_count < resolv::MAX_SERVERS;
       ++i) {
    if (state.nsaddr_list[i].sin_family != AF_INET) {
      if (static_cast<size_t>(i) < from_file.server_count)
        out.servers[out.server_count++] = from_file.servers[i];
      continue;
    }
    resolv::Nameserver &server = out.servers[out.server_count++];
    server.family = AF_INET;
    server.port = state.nsaddr_list[i].sin_port;
    inline_memcpy(server.bytes, &state.nsaddr_list[i].sin_addr, 4);
  }
}

} // namespace internal
} // namespace LIBC_NAMESPACE_DECL
