//===-- Reading and writing Ethernet addresses ------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_NETINET_ETHER_ETHERS_H
#define LLVM_LIBC_SRC_NETINET_ETHER_ETHERS_H

#include "hdr/types/size_t.h"
#include "hdr/types/struct_ether_addr.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {
namespace ether {

// The longest an address ever is written: six pairs of digits and five
// separators, and the terminator.
constexpr size_t TEXT_SIZE = 18;

// Reads an address written as six hexadecimal numbers separated by colons.
bool from_text(const char *asc, struct ether_addr *addr);

// Writes an address the way from_text reads it. |buf| holds TEXT_SIZE bytes.
void to_text(const struct ether_addr *addr, char *buf);

// Reads one line of the ethers database: an address, then the name of the
// machine it belongs to. Returns false where the line holds neither, which a
// blank line or one that is all comment does.
bool from_line(const char *line, struct ether_addr *addr, char *hostname);

// Overrides the ethers file path, for testing.
void TESTONLY_set_ethers_path(const char *path);

// Searches the ethers database. One of the two is given and the other is
// filled in.
bool find_by_addr(const struct ether_addr *addr, char *hostname);
bool find_by_name(const char *hostname, struct ether_addr *addr);

} // namespace ether
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_NETINET_ETHER_ETHERS_H
