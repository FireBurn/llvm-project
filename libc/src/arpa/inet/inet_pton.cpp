//===-- Implementation of inet_pton ---------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/arpa/inet/inet_pton.h"

#include "hdr/errno_macros.h"
#include "hdr/sys_socket_macros.h"
#include "hdr/types/size_t.h"
#include "src/__support/common.h"
#include "src/__support/ctype_utils.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

namespace {

// Unlike inet_addr, this takes only the four part dotted form, with each
// part in decimal, no leading zeros and no more than 255.
bool parse_ipv4(const char *src, unsigned char out[4]) {
  int part = 0;
  for (;;) {
    if (!internal::isdigit(static_cast<unsigned char>(*src)))
      return false;

    const bool leading_zero = *src == '0';
    unsigned value = 0;
    int digits = 0;
    while (internal::isdigit(static_cast<unsigned char>(*src))) {
      value = value * 10 + static_cast<unsigned>(*src - '0');
      if (++digits > 3 || value > 255)
        return false;
      ++src;
    }
    // "0" is a part; "01" is not.
    if (leading_zero && digits > 1)
      return false;
    out[part] = static_cast<unsigned char>(value);

    if (++part == 4)
      return *src == '\0';
    if (*src != '.')
      return false;
    ++src;
  }
}

int hex_value(char c) {
  if (c >= '0' && c <= '9')
    return c - '0';
  if (c >= 'a' && c <= 'f')
    return c - 'a' + 10;
  if (c >= 'A' && c <= 'F')
    return c - 'A' + 10;
  return -1;
}

// Whether what follows, up to the next colon or the end, holds a dot, which
// is how an address written with its last two groups as IPv4 is told apart.
bool looks_like_ipv4(const char *src) {
  for (; *src != '\0' && *src != ':'; ++src)
    if (*src == '.')
      return true;
  return false;
}

// Eight groups of up to four hex digits separated by colons, with one run
// of zero groups allowed to be left out as "::", and the last two groups
// allowed to be written as an IPv4 address instead.
bool parse_ipv6(const char *src, unsigned char out[16]) {
  unsigned char buf[16] = {};
  int filled = 0;
  // Where the run of zero groups was left out, or -1 if it was not.
  int gap = -1;

  if (src[0] == ':') {
    // A leading colon is only allowed as the first half of "::".
    if (src[1] != ':')
      return false;
    src += 2;
    gap = 0;
  }

  while (*src != '\0') {
    if (looks_like_ipv4(src)) {
      // This is the last thing in the address, and it takes two groups.
      if (filled + 4 > 16)
        return false;
      if (!parse_ipv4(src, buf + filled))
        return false;
      filled += 4;
      src = "";
      break;
    }

    unsigned value = 0;
    int digits = 0;
    while (hex_value(*src) >= 0) {
      value = (value << 4) | static_cast<unsigned>(hex_value(*src));
      if (++digits > 4)
        return false;
      ++src;
    }
    if (digits == 0)
      return false;
    if (filled + 2 > 16)
      return false;
    buf[filled++] = static_cast<unsigned char>(value >> 8);
    buf[filled++] = static_cast<unsigned char>(value & 0xff);

    if (*src == '\0')
      break;
    if (*src != ':')
      return false;
    ++src;
    if (*src == ':') {
      if (gap >= 0)
        return false; // Only one run may be left out.
      gap = filled;
      ++src;
      if (*src == '\0')
        break;
    } else if (*src == '\0') {
      return false; // A colon with nothing after it.
    }
  }

  if (gap >= 0) {
    // What was read after the gap belongs at the end, and what the gap
    // stands for is zero. It has to stand for at least one group.
    if (filled == 16)
      return false;
    const int tail = filled - gap;
    for (int i = 1; i <= tail; ++i) {
      buf[16 - i] = buf[filled - i];
      buf[filled - i] = 0;
    }
  } else if (filled != 16) {
    return false;
  }

  for (int i = 0; i < 16; ++i)
    out[i] = buf[i];
  return true;
}

} // anonymous namespace

// Returns 1 for an address it could read, 0 for one it could not, and -1
// with EAFNOSUPPORT for a family it does not know. Nothing is written to
// dst unless the whole string parsed.
LLVM_LIBC_FUNCTION(int, inet_pton,
                   (int af, const char *__restrict src, void *__restrict dst)) {
  if (src == nullptr || dst == nullptr)
    return 0;

  if (af == AF_INET) {
    unsigned char out[4];
    if (!parse_ipv4(src, out))
      return 0;
    for (int i = 0; i < 4; ++i)
      static_cast<unsigned char *>(dst)[i] = out[i];
    return 1;
  }

  if (af == AF_INET6) {
    unsigned char out[16];
    if (!parse_ipv6(src, out))
      return 0;
    for (int i = 0; i < 16; ++i)
      static_cast<unsigned char *>(dst)[i] = out[i];
    return 1;
  }

  libc_errno = EAFNOSUPPORT;
  return -1;
}

} // namespace LIBC_NAMESPACE_DECL
