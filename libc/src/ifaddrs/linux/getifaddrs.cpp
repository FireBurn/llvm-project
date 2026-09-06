//===-- Linux implementation of getifaddrs --------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/ifaddrs/getifaddrs.h"

#include "hdr/errno_macros.h"
#include "hdr/func/free.h"
#include "hdr/func/malloc.h"
#include "hdr/net_if_macros.h"
#include "hdr/sys_socket_macros.h"
#include "hdr/types/socklen_t.h"
#include "hdr/types/ssize_t.h"
#include "hdr/types/struct_ifaddrs.h"
#include "hdr/types/struct_sockaddr.h"
#include "hdr/types/struct_sockaddr_in.h"
#include "hdr/types/struct_sockaddr_in6.h"
#include "include/llvm-libc-types/struct_sockaddr_ll.h"
#include "src/__support/CPP/scope.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/close.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/recvfrom.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/sendto.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/socket.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/string/memory_utils/inline_memcpy.h"
#include "src/string/memory_utils/inline_memset.h"
#include "src/string/string_utils.h"

#include <linux/if_link.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>

namespace LIBC_NAMESPACE_DECL {

namespace {

// What one recvfrom asks for at a time. A dump arrives in as many messages as
// it takes, and they are gathered into one buffer as they come.
constexpr size_t CHUNK = 16384;

// A buffer which grows as a dump is read into it.
struct Dump {
  uint8_t *data = nullptr;
  size_t size = 0;
  size_t capacity = 0;

  bool reserve(size_t needed) {
    if (needed <= capacity)
      return true;
    size_t wanted = capacity == 0 ? CHUNK * 2 : capacity * 2;
    if (wanted < needed)
      wanted = needed;
    auto *bigger = static_cast<uint8_t *>(malloc(wanted));
    if (bigger == nullptr)
      return false;
    if (data != nullptr) {
      inline_memcpy(bigger, data, size);
      free(data);
    }
    data = bigger;
    capacity = wanted;
    return true;
  }

  void release() {
    free(data);
    data = nullptr;
    size = capacity = 0;
  }
};

// Asks for every message of one kind and reads the answer to the end. The
// kernel answers a dump in as many messages as it needs, and the last one is
// NLMSG_DONE.
int read_dump(int fd, uint16_t type, uint32_t seq, Dump &into) {
  struct {
    struct nlmsghdr nlh;
    struct rtgenmsg gen;
  } request = {};
  request.nlh.nlmsg_len = NLMSG_LENGTH(sizeof(request.gen));
  request.nlh.nlmsg_type = type;
  request.nlh.nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;
  request.nlh.nlmsg_seq = seq;
  request.gen.rtgen_family = AF_UNSPEC;

  auto sent = linux_syscalls::sendto(fd, &request, request.nlh.nlmsg_len, 0,
                                     nullptr, 0);
  if (!sent.has_value())
    return sent.error();

  for (;;) {
    if (!into.reserve(into.size + CHUNK))
      return ENOMEM;
    auto got = linux_syscalls::recvfrom(fd, into.data + into.size, CHUNK, 0,
                                        nullptr, nullptr);
    if (!got.has_value())
      return got.error();
    if (got.value() == 0)
      return 0; // The other end went away; what arrived is what there is.

    const size_t received = static_cast<size_t>(got.value());
    // Whether this batch ends the dump has to be known before the buffer is
    // grown again, since the header walk is over this batch alone.
    bool done = false;
    size_t left = received;
    for (auto *nh = reinterpret_cast<struct nlmsghdr *>(into.data + into.size);
         NLMSG_OK(nh, left); nh = NLMSG_NEXT(nh, left)) {
      if (nh->nlmsg_type == NLMSG_DONE) {
        done = true;
        break;
      }
      if (nh->nlmsg_type == NLMSG_ERROR) {
        if (nh->nlmsg_len < NLMSG_LENGTH(sizeof(struct nlmsgerr)))
          return EINVAL;
        auto *err = reinterpret_cast<struct nlmsgerr *>(NLMSG_DATA(nh));
        if (err->error != 0)
          return -err->error;
        done = true;
        break;
      }
    }
    into.size += received;
    if (done)
      return 0;
  }
}

// How much room a socket address of this family takes. An address the library
// has no name for is not reported at all, which is what every other
// implementation does with one.
size_t sockaddr_size(sa_family_t family) {
  switch (family) {
  case AF_INET:
    return sizeof(struct sockaddr_in);
  case AF_INET6:
    return sizeof(struct sockaddr_in6);
  case AF_PACKET:
    return sizeof(struct sockaddr_ll);
  default:
    return 0;
  }
}

// Writes an address of `family` from `data`, which is the raw address the
// kernel gave, and answers with what was written.
void fill_sockaddr(void *where, sa_family_t family, const void *data,
                   size_t length) {
  inline_memset(where, 0, sockaddr_size(family));
  if (family == AF_INET) {
    auto *sin = static_cast<struct sockaddr_in *>(where);
    sin->sin_family = AF_INET;
    inline_memcpy(&sin->sin_addr, data,
                  length < sizeof(sin->sin_addr) ? length
                                                 : sizeof(sin->sin_addr));
  } else if (family == AF_INET6) {
    auto *sin6 = static_cast<struct sockaddr_in6 *>(where);
    sin6->sin6_family = AF_INET6;
    inline_memcpy(&sin6->sin6_addr, data,
                  length < sizeof(sin6->sin6_addr) ? length
                                                   : sizeof(sin6->sin6_addr));
  }
}

// The netmask a prefix length stands for, written out as an address of the
// same family.
void fill_netmask(void *where, sa_family_t family, unsigned prefix) {
  inline_memset(where, 0, sockaddr_size(family));
  uint8_t *bytes = nullptr;
  size_t total_bits = 0;
  if (family == AF_INET) {
    auto *sin = static_cast<struct sockaddr_in *>(where);
    sin->sin_family = AF_INET;
    bytes = reinterpret_cast<uint8_t *>(&sin->sin_addr);
    total_bits = 32;
  } else if (family == AF_INET6) {
    auto *sin6 = static_cast<struct sockaddr_in6 *>(where);
    sin6->sin6_family = AF_INET6;
    bytes = reinterpret_cast<uint8_t *>(&sin6->sin6_addr);
    total_bits = 128;
  } else {
    return;
  }
  if (prefix > total_bits)
    prefix = static_cast<unsigned>(total_bits);
  for (size_t i = 0; prefix > 0; ++i) {
    const unsigned here = prefix >= 8 ? 8 : prefix;
    bytes[i] = static_cast<uint8_t>(0xffu << (8 - here));
    prefix -= here;
  }
}

// What is known about one interface, gathered from the link dump so that the
// address entries can be given a name and flags.
struct Link {
  int index;
  unsigned int flags;
  const char *name; // Into the dump buffer, which outlives the walk.
  size_t name_length;
  const void *hwaddr;
  size_t hwaddr_length;
  const void *broadcast;
  size_t broadcast_length;
  unsigned short type;
};

// Walks a dump, calling `each(nh)` for every message of `wanted`.
template <typename F>
void for_each_message(const Dump &d, uint16_t wanted, F each) {
  size_t left = d.size;
  for (auto *nh = reinterpret_cast<struct nlmsghdr *>(d.data);
       NLMSG_OK(nh, left); nh = NLMSG_NEXT(nh, left)) {
    if (nh->nlmsg_type == NLMSG_DONE)
      break;
    if (nh->nlmsg_type == wanted)
      each(nh);
  }
}

} // anonymous namespace

// The addresses of every interface, as one list the caller frees with
// freeifaddrs. The list is one allocation: the entries, then the names, then
// the addresses, so that giving it back is a single call.
LLVM_LIBC_FUNCTION(int, getifaddrs, (struct ifaddrs * *list)) {
  if (list == nullptr) {
    libc_errno = EINVAL;
    return -1;
  }
  *list = nullptr;

  auto fd_or_err = linux_syscalls::socket(AF_NETLINK, SOCK_RAW | SOCK_CLOEXEC,
                                          NETLINK_ROUTE);
  if (!fd_or_err.has_value()) {
    libc_errno = fd_or_err.error();
    return -1;
  }
  const int fd = fd_or_err.value();

  Dump links, addrs;
  cpp::scope_exit cleanup([&]() {
    links.release();
    addrs.release();
    linux_syscalls::close(fd);
  });

  if (int err = read_dump(fd, RTM_GETLINK, 1, links); err != 0) {
    libc_errno = err;
    return -1;
  }
  if (int err = read_dump(fd, RTM_GETADDR, 2, addrs); err != 0) {
    libc_errno = err;
    return -1;
  }

  // What every entry needs: one for each interface, and one more for each
  // address on it.
  size_t entries = 0, names_bytes = 0, addr_bytes = 0;

  auto link_of = [&links](int index) -> Link {
    Link found = {};
    found.index = -1;
    for_each_message(links, RTM_NEWLINK, [&](struct nlmsghdr *nh) {
      if (found.index >= 0 ||
          nh->nlmsg_len < NLMSG_LENGTH(sizeof(struct ifinfomsg)))
        return;
      auto *ifi = reinterpret_cast<struct ifinfomsg *>(NLMSG_DATA(nh));
      if (ifi->ifi_index != index)
        return;
      found.index = ifi->ifi_index;
      found.flags = ifi->ifi_flags;
      found.type = ifi->ifi_type;
      size_t attrlen = nh->nlmsg_len - NLMSG_LENGTH(sizeof(struct ifinfomsg));
      for (struct rtattr *rta = IFLA_RTA(ifi); RTA_OK(rta, attrlen);
           rta = RTA_NEXT(rta, attrlen)) {
        if (rta->rta_type == IFLA_IFNAME) {
          found.name = static_cast<const char *>(RTA_DATA(rta));
          found.name_length = internal::strnlen(found.name, RTA_PAYLOAD(rta));
        } else if (rta->rta_type == IFLA_ADDRESS) {
          found.hwaddr = RTA_DATA(rta);
          found.hwaddr_length = RTA_PAYLOAD(rta);
        } else if (rta->rta_type == IFLA_BROADCAST) {
          found.broadcast = RTA_DATA(rta);
          found.broadcast_length = RTA_PAYLOAD(rta);
        }
      }
    });
    return found;
  };

  // One entry per interface, carrying its hardware address.
  for_each_message(links, RTM_NEWLINK, [&](struct nlmsghdr *nh) {
    if (nh->nlmsg_len < NLMSG_LENGTH(sizeof(struct ifinfomsg)))
      return;
    auto *ifi = reinterpret_cast<struct ifinfomsg *>(NLMSG_DATA(nh));
    Link link = link_of(ifi->ifi_index);
    if (link.name == nullptr)
      return;
    ++entries;
    names_bytes += link.name_length + 1;
    addr_bytes += sizeof(struct sockaddr_ll);
    if (link.broadcast != nullptr)
      addr_bytes += sizeof(struct sockaddr_ll);
  });

  // One entry per address.
  for_each_message(addrs, RTM_NEWADDR, [&](struct nlmsghdr *nh) {
    if (nh->nlmsg_len < NLMSG_LENGTH(sizeof(struct ifaddrmsg)))
      return;
    auto *ifa = reinterpret_cast<struct ifaddrmsg *>(NLMSG_DATA(nh));
    const size_t one = sockaddr_size(ifa->ifa_family);
    if (one == 0)
      return;
    Link link = link_of(static_cast<int>(ifa->ifa_index));
    if (link.name == nullptr)
      return;
    ++entries;
    names_bytes += link.name_length + 1;
    addr_bytes += one * 3; // The address, its netmask and the broadcast.
  });

  if (entries == 0) {
    *list = nullptr;
    return 0;
  }

  // Everything in one block, so that freeing it is one call.
  const size_t entries_bytes = entries * sizeof(struct ifaddrs);
  const size_t total = entries_bytes + names_bytes + addr_bytes;
  auto *block = static_cast<uint8_t *>(malloc(total));
  if (block == nullptr) {
    libc_errno = ENOMEM;
    return -1;
  }
  inline_memset(block, 0, total);

  auto *out = reinterpret_cast<struct ifaddrs *>(block);
  char *names = reinterpret_cast<char *>(block + entries_bytes);
  uint8_t *sockaddrs = block + entries_bytes + names_bytes;
  size_t used = 0;

  auto take_name = [&names](const char *name, size_t length) {
    char *where = names;
    inline_memcpy(where, name, length);
    where[length] = '\0';
    names += length + 1;
    return where;
  };
  auto take_addr = [&sockaddrs](size_t bytes) {
    uint8_t *where = sockaddrs;
    sockaddrs += bytes;
    return where;
  };

  for_each_message(links, RTM_NEWLINK, [&](struct nlmsghdr *nh) {
    if (nh->nlmsg_len < NLMSG_LENGTH(sizeof(struct ifinfomsg)))
      return;
    auto *ifi = reinterpret_cast<struct ifinfomsg *>(NLMSG_DATA(nh));
    Link link = link_of(ifi->ifi_index);
    if (link.name == nullptr || used >= entries)
      return;

    struct ifaddrs *entry = &out[used++];
    entry->ifa_name = take_name(link.name, link.name_length);
    entry->ifa_flags = link.flags;

    auto *sll = reinterpret_cast<struct sockaddr_ll *>(
        take_addr(sizeof(struct sockaddr_ll)));
    sll->sll_family = AF_PACKET;
    sll->sll_ifindex = link.index;
    sll->sll_hatype = link.type;
    if (link.hwaddr != nullptr) {
      size_t n = link.hwaddr_length;
      if (n > sizeof(sll->sll_addr))
        n = sizeof(sll->sll_addr);
      sll->sll_halen = static_cast<unsigned char>(n);
      inline_memcpy(sll->sll_addr, link.hwaddr, n);
    }
    entry->ifa_addr = reinterpret_cast<struct sockaddr *>(sll);

    if (link.broadcast != nullptr) {
      auto *bll = reinterpret_cast<struct sockaddr_ll *>(
          take_addr(sizeof(struct sockaddr_ll)));
      bll->sll_family = AF_PACKET;
      bll->sll_ifindex = link.index;
      bll->sll_hatype = link.type;
      size_t n = link.broadcast_length;
      if (n > sizeof(bll->sll_addr))
        n = sizeof(bll->sll_addr);
      bll->sll_halen = static_cast<unsigned char>(n);
      inline_memcpy(bll->sll_addr, link.broadcast, n);
      entry->ifa_broadaddr = reinterpret_cast<struct sockaddr *>(bll);
    }
  });

  for_each_message(addrs, RTM_NEWADDR, [&](struct nlmsghdr *nh) {
    if (nh->nlmsg_len < NLMSG_LENGTH(sizeof(struct ifaddrmsg)))
      return;
    auto *ifa = reinterpret_cast<struct ifaddrmsg *>(NLMSG_DATA(nh));
    const size_t one = sockaddr_size(ifa->ifa_family);
    if (one == 0)
      return;
    Link link = link_of(static_cast<int>(ifa->ifa_index));
    if (link.name == nullptr || used >= entries)
      return;

    const void *address = nullptr;
    size_t address_length = 0;
    const void *broadcast = nullptr;
    size_t broadcast_length = 0;
    const char *label = nullptr;
    size_t label_length = 0;

    size_t attrlen = nh->nlmsg_len - NLMSG_LENGTH(sizeof(struct ifaddrmsg));
    for (struct rtattr *rta = IFA_RTA(ifa); RTA_OK(rta, attrlen);
         rta = RTA_NEXT(rta, attrlen)) {
      switch (rta->rta_type) {
      case IFA_LOCAL:
        // On a point to point interface this is the near end, which is the
        // one the address of the interface means.
        address = RTA_DATA(rta);
        address_length = RTA_PAYLOAD(rta);
        break;
      case IFA_ADDRESS:
        if (address == nullptr) {
          address = RTA_DATA(rta);
          address_length = RTA_PAYLOAD(rta);
        } else {
          broadcast = RTA_DATA(rta);
          broadcast_length = RTA_PAYLOAD(rta);
        }
        break;
      case IFA_BROADCAST:
        broadcast = RTA_DATA(rta);
        broadcast_length = RTA_PAYLOAD(rta);
        break;
      case IFA_LABEL:
        label = static_cast<const char *>(RTA_DATA(rta));
        label_length = internal::strnlen(label, RTA_PAYLOAD(rta));
        break;
      default:
        break;
      }
    }
    if (address == nullptr)
      return;

    struct ifaddrs *entry = &out[used++];
    // An alias carries a label of its own, which is the name it goes by.
    if (label != nullptr)
      entry->ifa_name = take_name(label, label_length);
    else
      entry->ifa_name = take_name(link.name, link.name_length);
    entry->ifa_flags = link.flags;

    void *where = take_addr(one);
    fill_sockaddr(where, ifa->ifa_family, address, address_length);
    entry->ifa_addr = static_cast<struct sockaddr *>(where);

    void *mask = take_addr(one);
    fill_netmask(mask, ifa->ifa_family, ifa->ifa_prefixlen);
    entry->ifa_netmask = static_cast<struct sockaddr *>(mask);

    if (broadcast != nullptr) {
      void *other = take_addr(one);
      fill_sockaddr(other, ifa->ifa_family, broadcast, broadcast_length);
      entry->ifa_broadaddr = static_cast<struct sockaddr *>(other);
    }
  });

  for (size_t i = 0; i + 1 < used; ++i)
    out[i].ifa_next = &out[i + 1];
  if (used == 0) {
    free(block);
    *list = nullptr;
    return 0;
  }
  out[used - 1].ifa_next = nullptr;

  *list = out;
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL
