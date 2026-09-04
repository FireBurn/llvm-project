//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// The open conversion iconv_open hands back, and the two halves it is made
/// of. Every conversion goes through a code point, so a new character set
/// only needs to say how its bytes and a code point correspond.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_ICONV_CONVERSION_H
#define LLVM_LIBC_SRC_ICONV_CONVERSION_H

#include "hdr/errno_macros.h"
#include "hdr/stdint_proxy.h"
#include "hdr/types/char32_t.h"
#include "hdr/types/size_t.h"
#include "src/__support/ctype_utils.h"
#include "src/__support/macros/attributes.h"
#include "src/__support/macros/config.h"
#include "src/iconv/charsets.h"

namespace LIBC_NAMESPACE_DECL {
namespace iconv_internal {

struct Conversion {
  Encoding from;
  Encoding to;
  const uint16_t *from_table;
  const uint16_t *to_table;
};

// A name matches without regard to case, or to the punctuation between its
// parts, so "ISO-8859-1" and "iso88591" name the same set. A trailing
// "//TRANSLIT" or "//IGNORE" is not part of the name.
LIBC_INLINE bool same_name(const char *wanted, const char *known) {
  const char *w = wanted;
  const char *k = known;
  for (;;) {
    while (*w == '-' || *w == '_' || *w == ' ')
      ++w;
    // Everything from a "//" onwards asks for a behaviour, not a set.
    if (w[0] == '/' && w[1] == '/')
      break;
    if (*w == '\0')
      break;
    if (*k == '\0')
      return false;
    if (internal::toupper(*w) != *k)
      return false;
    ++w;
    ++k;
  }
  while (*w == '-' || *w == '_' || *w == ' ')
    ++w;
  return *k == '\0' && (*w == '\0' || (w[0] == '/' && w[1] == '/'));
}

LIBC_INLINE const Charset *find_charset(const char *name) {
  if (name == nullptr)
    return nullptr;
  for (size_t i = 0; i < CHARSET_COUNT; ++i)
    if (same_name(name, CHARSETS[i].name))
      return &CHARSETS[i];
  return nullptr;
}

// What a step of a conversion ended up doing.
enum class Status {
  OK,
  INCOMPLETE, // The input ran out part way through a character.
  INVALID,    // The bytes are not a character in this set.
  FULL,       // There is no room in the output.
};

// Reads one character. |used| is how many bytes it took.
LIBC_INLINE Status decode(const Conversion &conv, const unsigned char *in,
                          size_t inleft, char32_t &out, size_t &used) {
  if (inleft == 0)
    return Status::INCOMPLETE;

  switch (conv.from) {
  case Encoding::ASCII:
    if (in[0] > 0x7F)
      return Status::INVALID;
    out = in[0];
    used = 1;
    return Status::OK;

  case Encoding::SINGLE_BYTE: {
    if (in[0] < 0x80) {
      out = in[0];
      used = 1;
      return Status::OK;
    }
    uint16_t mapped = conv.from_table[in[0] - 0x80];
    if (mapped == UNASSIGNED)
      return Status::INVALID;
    out = mapped;
    used = 1;
    return Status::OK;
  }

  case Encoding::UTF8: {
    unsigned char lead = in[0];
    size_t length;
    char32_t value;
    if (lead < 0x80) {
      out = lead;
      used = 1;
      return Status::OK;
    } else if ((lead & 0xE0) == 0xC0) {
      length = 2;
      value = lead & 0x1F;
    } else if ((lead & 0xF0) == 0xE0) {
      length = 3;
      value = lead & 0x0F;
    } else if ((lead & 0xF8) == 0xF0) {
      length = 4;
      value = lead & 0x07;
    } else {
      return Status::INVALID;
    }
    if (inleft < length)
      return Status::INCOMPLETE;
    for (size_t i = 1; i < length; ++i) {
      if ((in[i] & 0xC0) != 0x80)
        return Status::INVALID;
      value = (value << 6) | (in[i] & 0x3F);
    }
    // A character written in more bytes than it needs, a surrogate, or one
    // past the end of Unicode is not a character.
    static constexpr char32_t LOWEST[5] = {0, 0, 0x80, 0x800, 0x10000};
    if (value < LOWEST[length] || value > 0x10FFFF ||
        (value >= 0xD800 && value <= 0xDFFF))
      return Status::INVALID;
    out = value;
    used = length;
    return Status::OK;
  }

  case Encoding::UTF16LE:
  case Encoding::UTF16BE: {
    if (inleft < 2)
      return Status::INCOMPLETE;
    const bool big = conv.from == Encoding::UTF16BE;
    auto unit = [big](const unsigned char *p) -> uint32_t {
      return big ? (uint32_t(p[0]) << 8) | p[1] : (uint32_t(p[1]) << 8) | p[0];
    };
    uint32_t first = unit(in);
    if (first < 0xD800 || first > 0xDFFF) {
      out = first;
      used = 2;
      return Status::OK;
    }
    if (first >= 0xDC00)
      return Status::INVALID; // A low surrogate with no high one before it.
    if (inleft < 4)
      return Status::INCOMPLETE;
    uint32_t second = unit(in + 2);
    if (second < 0xDC00 || second > 0xDFFF)
      return Status::INVALID;
    out = 0x10000 + ((first - 0xD800) << 10) + (second - 0xDC00);
    used = 4;
    return Status::OK;
  }

  case Encoding::UTF32LE:
  case Encoding::UTF32BE: {
    if (inleft < 4)
      return Status::INCOMPLETE;
    uint32_t value;
    if (conv.from == Encoding::UTF32BE)
      value = (uint32_t(in[0]) << 24) | (uint32_t(in[1]) << 16) |
              (uint32_t(in[2]) << 8) | in[3];
    else
      value = (uint32_t(in[3]) << 24) | (uint32_t(in[2]) << 16) |
              (uint32_t(in[1]) << 8) | in[0];
    if (value > 0x10FFFF || (value >= 0xD800 && value <= 0xDFFF))
      return Status::INVALID;
    out = value;
    used = 4;
    return Status::OK;
  }
  }
  return Status::INVALID;
}

// Writes one character. |made| is how many bytes it took.
LIBC_INLINE Status encode(const Conversion &conv, char32_t cp,
                          unsigned char *out, size_t outleft, size_t &made) {
  switch (conv.to) {
  case Encoding::ASCII:
    if (cp > 0x7F)
      return Status::INVALID;
    if (outleft < 1)
      return Status::FULL;
    out[0] = static_cast<unsigned char>(cp);
    made = 1;
    return Status::OK;

  case Encoding::SINGLE_BYTE: {
    if (cp < 0x80) {
      if (outleft < 1)
        return Status::FULL;
      out[0] = static_cast<unsigned char>(cp);
      made = 1;
      return Status::OK;
    }
    if (cp > 0xFFFF)
      return Status::INVALID;
    // The table is small enough that a walk costs less than a second table
    // to invert it would.
    for (size_t i = 0; i < 128; ++i) {
      if (conv.to_table[i] != cp)
        continue;
      if (outleft < 1)
        return Status::FULL;
      out[0] = static_cast<unsigned char>(0x80 + i);
      made = 1;
      return Status::OK;
    }
    return Status::INVALID;
  }

  case Encoding::UTF8: {
    size_t length = cp < 0x80 ? 1 : cp < 0x800 ? 2 : cp < 0x10000 ? 3 : 4;
    if (outleft < length)
      return Status::FULL;
    switch (length) {
    case 1:
      out[0] = static_cast<unsigned char>(cp);
      break;
    case 2:
      out[0] = static_cast<unsigned char>(0xC0 | (cp >> 6));
      out[1] = static_cast<unsigned char>(0x80 | (cp & 0x3F));
      break;
    case 3:
      out[0] = static_cast<unsigned char>(0xE0 | (cp >> 12));
      out[1] = static_cast<unsigned char>(0x80 | ((cp >> 6) & 0x3F));
      out[2] = static_cast<unsigned char>(0x80 | (cp & 0x3F));
      break;
    default:
      out[0] = static_cast<unsigned char>(0xF0 | (cp >> 18));
      out[1] = static_cast<unsigned char>(0x80 | ((cp >> 12) & 0x3F));
      out[2] = static_cast<unsigned char>(0x80 | ((cp >> 6) & 0x3F));
      out[3] = static_cast<unsigned char>(0x80 | (cp & 0x3F));
      break;
    }
    made = length;
    return Status::OK;
  }

  case Encoding::UTF16LE:
  case Encoding::UTF16BE: {
    const bool big = conv.to == Encoding::UTF16BE;
    auto put = [big](unsigned char *p, uint32_t unit) {
      p[big ? 0 : 1] = static_cast<unsigned char>(unit >> 8);
      p[big ? 1 : 0] = static_cast<unsigned char>(unit & 0xFF);
    };
    if (cp < 0x10000) {
      if (outleft < 2)
        return Status::FULL;
      put(out, cp);
      made = 2;
      return Status::OK;
    }
    if (outleft < 4)
      return Status::FULL;
    uint32_t rest = cp - 0x10000;
    put(out, 0xD800 + (rest >> 10));
    put(out + 2, 0xDC00 + (rest & 0x3FF));
    made = 4;
    return Status::OK;
  }

  case Encoding::UTF32LE:
  case Encoding::UTF32BE: {
    if (outleft < 4)
      return Status::FULL;
    if (conv.to == Encoding::UTF32BE) {
      out[0] = static_cast<unsigned char>(cp >> 24);
      out[1] = static_cast<unsigned char>((cp >> 16) & 0xFF);
      out[2] = static_cast<unsigned char>((cp >> 8) & 0xFF);
      out[3] = static_cast<unsigned char>(cp & 0xFF);
    } else {
      out[0] = static_cast<unsigned char>(cp & 0xFF);
      out[1] = static_cast<unsigned char>((cp >> 8) & 0xFF);
      out[2] = static_cast<unsigned char>((cp >> 16) & 0xFF);
      out[3] = static_cast<unsigned char>(cp >> 24);
    }
    made = 4;
    return Status::OK;
  }
  }
  return Status::INVALID;
}

} // namespace iconv_internal
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_ICONV_CONVERSION_H
