//===-- Turning a name into addresses ---------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_NETDB_RESOLV_LOOKUP_H
#define LLVM_LIBC_SRC_NETDB_RESOLV_LOOKUP_H

#include "hdr/stdint_proxy.h"
#include "hdr/types/size_t.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {
namespace resolv {

// The longest name the domain name system allows, in the form people write.
constexpr size_t MAX_NAME = 255;
// How many addresses one lookup will report. The number is what the other
// libcs settled on: enough for any name people actually use, and small
// enough to keep on the stack.
constexpr size_t MAX_ADDRESSES = 48;

// One address, in whichever family it belongs to.
struct Address {
  int family;
  // Four bytes for an internet address, sixteen for a version six one.
  unsigned char bytes[16];
  // The scope a link local version six address belongs to, which is part of
  // naming it and zero for every other kind.
  uint32_t scope;
};

// Fills `out` with the addresses `name` stands for, in the order they should
// be tried, and returns how many there are. `family` limits the answer to one
// kind of address, or is AF_UNSPEC for both.
//
// On failure it returns one of getaddrinfo's EAI_ codes, which are negative,
// so anything below zero is an error and anything at or above it is a count.
// `canonical` receives the name the answer really came under, where that
// differs from the one asked about, and must hold MAX_NAME + 1 bytes. It is
// taken from the first answer, before the addresses are put in the order they
// should be tried.
int lookup_name(const char *name, int family, int flags, Address *out,
                size_t capacity, char *canonical);

// Puts the addresses of the newer kind first, which is what a machine with
// both should try first.
void prefer_newer_addresses(Address *out, size_t count);

} // namespace resolv
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_NETDB_RESOLV_LOOKUP_H
