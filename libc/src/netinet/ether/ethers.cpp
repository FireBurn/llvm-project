//===-- Reading and writing Ethernet addresses ----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/netinet/ether/ethers.h"

#include "src/__support/CPP/string_view.h"
#include "src/__support/ctype_utils.h"
#include "src/__support/macros/config.h"
#include "src/netdb/resolv/file_lines.h"
#include "src/string/memory_utils/inline_memcpy.h"

namespace LIBC_NAMESPACE_DECL {
namespace ether {

namespace {

constexpr const char *DEFAULT_ETHERS_PATH = "/etc/ethers";

const char *ethers_path = DEFAULT_ETHERS_PATH;

// The name of a machine can be as long as any host name.
constexpr size_t NAME_SIZE = 256;

// The value of one hexadecimal digit, or -1 where the character is not one.
int hex_digit(char ch) {
  if (ch >= '0' && ch <= '9')
    return ch - '0';
  if (ch >= 'a' && ch <= 'f')
    return ch - 'a' + 10;
  if (ch >= 'A' && ch <= 'F')
    return ch - 'A' + 10;
  return -1;
}

} // namespace

bool from_text(const char *asc, struct ether_addr *addr) {
  if (asc == nullptr || addr == nullptr)
    return false;

  uint8_t octets[ETH_ALEN];
  size_t at = 0;

  for (size_t i = 0; i < ETH_ALEN; ++i) {
    int digit = hex_digit(asc[at]);
    if (digit < 0)
      return false;
    unsigned value = static_cast<unsigned>(digit);
    ++at;

    // A part is written with one digit or two. What ends it says which: a
    // separator for all but the last, and the end of the text or a blank for
    // the last, since a line of the ethers database carries a name after it.
    const bool ends_here = i + 1 < ETH_ALEN
                               ? asc[at] == ':'
                               : asc[at] == '\0' || internal::isspace(asc[at]);
    if (!ends_here) {
      digit = hex_digit(asc[at]);
      if (digit < 0)
        return false;
      value = value * 16 + static_cast<unsigned>(digit);
      ++at;
      if (i + 1 < ETH_ALEN && asc[at] != ':')
        return false;
    }

    octets[i] = static_cast<uint8_t>(value);
    // Step over the separator. Past the last part there is nothing to step
    // over, and nothing there is looked at.
    ++at;
  }

  for (size_t i = 0; i < ETH_ALEN; ++i)
    addr->ether_addr_octet[i] = octets[i];
  return true;
}

void to_text(const struct ether_addr *addr, char *buf) {
  constexpr char DIGITS[] = "0123456789abcdef";
  size_t at = 0;
  for (size_t i = 0; i < ETH_ALEN; ++i) {
    if (i != 0)
      buf[at++] = ':';
    const uint8_t octet = addr->ether_addr_octet[i];
    // A leading zero is left off, which is what everything reading these
    // expects to see.
    if (octet >= 16)
      buf[at++] = DIGITS[octet >> 4];
    buf[at++] = DIGITS[octet & 0xf];
  }
  buf[at] = '\0';
}

bool from_line(const char *line, struct ether_addr *addr, char *hostname) {
  char copy[NAME_SIZE + TEXT_SIZE + 2];
  size_t length = 0;
  while (line[length] != '\0' && length + 1 < sizeof(copy)) {
    copy[length] = line[length];
    ++length;
  }
  copy[length] = '\0';
  resolv::drop_comment(copy);

  cpp::string_view rest(copy);
  cpp::string_view text = resolv::next_field(rest);
  if (text.empty() || text.size() >= TEXT_SIZE)
    return false;

  char address[TEXT_SIZE];
  inline_memcpy(address, text.data(), text.size());
  address[text.size()] = '\0';

  struct ether_addr parsed;
  if (!from_text(address, &parsed))
    return false;

  cpp::string_view name = resolv::next_field(rest);
  if (name.empty() || name.size() >= NAME_SIZE)
    return false;

  *addr = parsed;
  inline_memcpy(hostname, name.data(), name.size());
  hostname[name.size()] = '\0';
  return true;
}

namespace {

// Walks the database, handing each line that holds an entry to |matches|.
template <typename Match>
bool scan(struct ether_addr *addr, char *hostname, Match matches) {
  resolv::FileLines lines;
  if (!lines.open(ethers_path))
    return false;

  char line[NAME_SIZE + TEXT_SIZE + 2];
  bool found = false;
  while (lines.next_line(line, sizeof(line))) {
    struct ether_addr entry;
    char name[NAME_SIZE];
    if (!from_line(line, &entry, name))
      continue;
    if (!matches(entry, name))
      continue;
    *addr = entry;
    size_t i = 0;
    for (; name[i] != '\0'; ++i)
      hostname[i] = name[i];
    hostname[i] = '\0';
    found = true;
    break;
  }

  lines.close_file();
  return found;
}

bool same(const struct ether_addr &a, const struct ether_addr &b) {
  for (size_t i = 0; i < ETH_ALEN; ++i)
    if (a.ether_addr_octet[i] != b.ether_addr_octet[i])
      return false;
  return true;
}

} // namespace

void TESTONLY_set_ethers_path(const char *path) {
  ethers_path = path != nullptr ? path : DEFAULT_ETHERS_PATH;
}

bool find_by_addr(const struct ether_addr *addr, char *hostname) {
  if (addr == nullptr || hostname == nullptr)
    return false;
  struct ether_addr wanted = *addr;
  struct ether_addr found;
  return scan(&found, hostname,
              [&wanted](const struct ether_addr &entry, const char *) {
                return same(entry, wanted);
              });
}

bool find_by_name(const char *hostname, struct ether_addr *addr) {
  if (hostname == nullptr || addr == nullptr)
    return false;
  char name[NAME_SIZE];
  return scan(addr, name, [hostname](const struct ether_addr &, const char *n) {
    size_t i = 0;
    for (; hostname[i] != '\0' && n[i] != '\0'; ++i)
      if (hostname[i] != n[i])
        return false;
    return hostname[i] == '\0' && n[i] == '\0';
  });
}

} // namespace ether
} // namespace LIBC_NAMESPACE_DECL
