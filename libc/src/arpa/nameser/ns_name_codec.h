//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// The name handling the ns_name_ functions share: names in the wire form
/// (length-prefixed labels ending in a zero) and in the text form (labels
/// joined by dots, with backslash escapes).
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_ARPA_NAMESER_NS_NAME_CODEC_H
#define LLVM_LIBC_SRC_ARPA_NAMESER_NS_NAME_CODEC_H

#include "hdr/errno_macros.h"
#include "hdr/types/size_t.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {
namespace nameser {

constexpr size_t MAX_WIRE_NAME = 255;
constexpr size_t MAX_LABEL = 63;
constexpr unsigned char COMPRESSED = 0xc0;

LIBC_INLINE unsigned char lower(unsigned char c) {
  return (c >= 'A' && c <= 'Z') ? static_cast<unsigned char>(c + 32) : c;
}

LIBC_INLINE bool is_digit(unsigned char c) { return c >= '0' && c <= '9'; }

/// Writes the wire name `src` as text, which has a dot after each label and
/// a lone dot for the root. Returns the bytes written with the terminator,
/// or -1 with EMSGSIZE if `dst` is too small or `src` is not a plain name.
LIBC_INLINE int ntop(const unsigned char *src, char *dst, size_t dstsiz) {
  size_t out = 0;
  size_t total = 0;
  auto put = [&](char c) {
    if (out + 1 >= dstsiz)
      return false;
    dst[out++] = c;
    return true;
  };
  unsigned char len;
  while ((len = *src++) != 0) {
    if ((len & COMPRESSED) != 0 || len > MAX_LABEL ||
        (total += len + 1) >= MAX_WIRE_NAME) {
      libc_errno = EMSGSIZE;
      return -1;
    }
    if (out != 0 && !put('.')) {
      libc_errno = EMSGSIZE;
      return -1;
    }
    for (; len != 0; --len) {
      const unsigned char c = *src++;
      bool ok;
      if (c == '.' || c == '\\') {
        ok = put('\\') && put(static_cast<char>(c));
      } else if (c <= 0x20 || c >= 0x7f) {
        ok = put('\\') && put(static_cast<char>('0' + c / 100)) &&
             put(static_cast<char>('0' + (c / 10) % 10)) &&
             put(static_cast<char>('0' + c % 10));
      } else {
        ok = put(static_cast<char>(c));
      }
      if (!ok) {
        libc_errno = EMSGSIZE;
        return -1;
      }
    }
  }
  if (out == 0 && !put('.')) {
    libc_errno = EMSGSIZE;
    return -1;
  }
  if (out >= dstsiz) {
    libc_errno = EMSGSIZE;
    return -1;
  }
  dst[out++] = '\0';
  return static_cast<int>(out);
}

/// Writes the text name `src` in the wire form. Returns 1 if it ended in a
/// dot, 0 if it did not, and -1 with EMSGSIZE if it is not a valid name or
/// does not fit.
LIBC_INLINE int pton(const char *src, unsigned char *dst, size_t dstsiz) {
  size_t out = 1;
  size_t label = 0; // Where the length byte of the label being written is.
  if (dstsiz < 1) {
    libc_errno = EMSGSIZE;
    return -1;
  }
  if (dstsiz > MAX_WIRE_NAME)
    dstsiz = MAX_WIRE_NAME; // A wire name is no longer than this.
  if (src[0] == '.' && src[1] == '\0') {
    dst[0] = 0;
    return 1;
  }
  bool dotted = false;
  for (const unsigned char *p = reinterpret_cast<const unsigned char *>(src);
       *p != '\0'; ++p) {
    unsigned char c = *p;
    dotted = false;
    if (c == '.') {
      const size_t len = out - label - 1;
      if (len == 0 || len > MAX_LABEL) {
        libc_errno = EMSGSIZE;
        return -1;
      }
      dst[label] = static_cast<unsigned char>(len);
      label = out;
      if (out >= dstsiz) {
        libc_errno = EMSGSIZE;
        return -1;
      }
      ++out;
      dotted = true;
      continue;
    }
    if (c == '\\') {
      c = *++p;
      if (c == '\0') {
        libc_errno = EMSGSIZE;
        return -1;
      }
      if (is_digit(c)) {
        if (!is_digit(p[1]) || !is_digit(p[2])) {
          libc_errno = EMSGSIZE;
          return -1;
        }
        const int v = (c - '0') * 100 + (p[1] - '0') * 10 + (p[2] - '0');
        if (v > 255) {
          libc_errno = EMSGSIZE;
          return -1;
        }
        c = static_cast<unsigned char>(v);
        p += 2;
      }
    }
    if (out >= dstsiz) {
      libc_errno = EMSGSIZE;
      return -1;
    }
    dst[out++] = c;
  }
  if (dotted) {
    // The dot that ended it opened a label that is not there.
    --out;
    dst[out] = 0;
    return 1;
  }
  const size_t len = out - label - 1;
  if (len == 0 || len > MAX_LABEL || out >= dstsiz) {
    libc_errno = EMSGSIZE;
    return -1;
  }
  dst[label] = static_cast<unsigned char>(len);
  dst[out++] = 0;
  return 0;
}

/// Reads the wire name at `src`, which may end in or hold pointers into the
/// message, and writes it uncompressed to `dst`. Returns how many bytes it
/// takes at `src`, or -1 with EMSGSIZE if it is malformed or does not fit.
LIBC_INLINE int unpack(const unsigned char *msg, const unsigned char *eom,
                       const unsigned char *src, unsigned char *dst,
                       size_t dstsiz) {
  const unsigned char *p = src;
  size_t out = 0;
  int consumed = -1;
  int jumps = 0;
  for (;;) {
    if (p >= eom) {
      libc_errno = EMSGSIZE;
      return -1;
    }
    const unsigned char len = *p++;
    if (len == 0) {
      if (out >= dstsiz) {
        libc_errno = EMSGSIZE;
        return -1;
      }
      dst[out++] = 0;
      if (consumed < 0)
        consumed = static_cast<int>(p - src);
      return consumed;
    }
    if ((len & COMPRESSED) == COMPRESSED) {
      if (p >= eom) {
        libc_errno = EMSGSIZE;
        return -1;
      }
      const size_t target = ((len & 0x3fu) << 8) | *p++;
      if (consumed < 0)
        consumed = static_cast<int>(p - src);
      // A pointer has to go back, which also ends any loop.
      if (target >= static_cast<size_t>(eom - msg) || ++jumps > 127) {
        libc_errno = EMSGSIZE;
        return -1;
      }
      p = msg + target;
      continue;
    }
    if ((len & COMPRESSED) != 0 || len > MAX_LABEL || p + len > eom ||
        out + len + 1 >= dstsiz || out + len + 1 > MAX_WIRE_NAME) {
      libc_errno = EMSGSIZE;
      return -1;
    }
    dst[out++] = len;
    for (unsigned char i = 0; i < len; ++i)
      dst[out++] = *p++;
  }
}

/// Steps `*ptr` over the name there, which may end in a pointer. Returns 0,
/// or -1 with EMSGSIZE if the name is malformed.
LIBC_INLINE int skip(const unsigned char **ptr, const unsigned char *eom) {
  const unsigned char *p = *ptr;
  for (;;) {
    if (p >= eom) {
      libc_errno = EMSGSIZE;
      return -1;
    }
    const unsigned char len = *p++;
    if (len == 0)
      break;
    if ((len & COMPRESSED) == COMPRESSED) {
      if (p >= eom) {
        libc_errno = EMSGSIZE;
        return -1;
      }
      ++p;
      break;
    }
    if ((len & COMPRESSED) != 0 || p + len > eom) {
      libc_errno = EMSGSIZE;
      return -1;
    }
    p += len;
  }
  *ptr = p;
  return 0;
}

/// Whether the labels of the wire names at `a` and `b` are the same, with
/// letters matched in either case. `b` is in the message starting at `msg` and
/// may use pointers.
LIBC_INLINE bool suffix_matches(const unsigned char *a,
                                const unsigned char *msg,
                                const unsigned char *eom,
                                const unsigned char *b) {
  int jumps = 0;
  for (;;) {
    while (b < eom && (*b & COMPRESSED) == COMPRESSED) {
      if (b + 1 >= eom || ++jumps > 127)
        return false;
      const size_t target = ((*b & 0x3fu) << 8) | b[1];
      if (target >= static_cast<size_t>(eom - msg))
        return false;
      b = msg + target;
    }
    if (b >= eom)
      return false;
    if (*a != *b)
      return false;
    if (*a == 0)
      return true;
    if (b + *b + 1 > eom)
      return false;
    for (unsigned char i = 1; i <= *a; ++i)
      if (lower(a[i]) != lower(b[i]))
        return false;
    a += *a + 1;
    b += *b + 1;
  }
}

/// Writes the uncompressed wire name `src` to `dst`, as a pointer to an
/// earlier name where one in `dnptrs` has the same ending. `dnptrs[0]` is the
/// message's start, the entries up to the null are names already in it, and
/// the ones after `lastdnptr` are room for those this adds. Returns the bytes
/// written, or -1 with EMSGSIZE.
LIBC_INLINE int pack(const unsigned char *src, unsigned char *dst, int dstsiz,
                     const unsigned char **dnptrs,
                     const unsigned char **lastdnptr) {
  if (dstsiz < 0) {
    libc_errno = EMSGSIZE;
    return -1;
  }
  const unsigned char *msg = dnptrs != nullptr ? dnptrs[0] : nullptr;
  const unsigned char **free_slot = nullptr;
  if (dnptrs != nullptr) {
    free_slot = dnptrs + 1;
    while (*free_slot != nullptr)
      ++free_slot;
  }
  size_t out = 0;
  const size_t room = static_cast<size_t>(dstsiz);
  for (;;) {
    if (msg != nullptr && *src != 0) {
      for (const unsigned char **cand = dnptrs + 1; cand != free_slot; ++cand) {
        const unsigned char *at = *cand;
        // Only names already written can be pointed at.
        if (at < msg || at >= dst + out || at - msg >= 0x4000)
          continue;
        if (suffix_matches(src, msg, dst + out, at)) {
          if (out + 2 > room) {
            libc_errno = EMSGSIZE;
            return -1;
          }
          const size_t off = static_cast<size_t>(at - msg);
          dst[out++] = static_cast<unsigned char>(COMPRESSED | (off >> 8));
          dst[out++] = static_cast<unsigned char>(off & 0xff);
          return static_cast<int>(out);
        }
      }
      if (lastdnptr != nullptr && free_slot < lastdnptr - 1 &&
          dst + out - msg < 0x4000) {
        *free_slot++ = dst + out;
        *free_slot = nullptr;
      }
    }
    const unsigned char len = *src;
    if (len > MAX_LABEL || out + len + 1 > room) {
      libc_errno = EMSGSIZE;
      return -1;
    }
    dst[out++] = len;
    if (len == 0)
      return static_cast<int>(out);
    ++src;
    for (unsigned char i = 0; i < len; ++i)
      dst[out++] = *src++;
  }
}

} // namespace nameser
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_ARPA_NAMESER_NS_NAME_CODEC_H
