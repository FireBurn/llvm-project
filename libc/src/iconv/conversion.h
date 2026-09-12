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
#include "src/__support/endian_internal.h"
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
  // Whether what cannot be converted is left out rather than reported.
  bool ignore;
  // For input which may begin with a byte order mark: whether one may still
  // come, and the order the input is read in.
  bool read_mark;
  bool read_big;
  // For output which begins with a byte order mark: whether it is still to be
  // written.
  bool write_mark;
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

// Whether the flags after a target set's name ask for |flag|. The flags follow
// "//", are separated by "//" or ",", and match without regard to case. One
// which is not known is not an error.
LIBC_INLINE bool has_flag(const char *name, const char *flag) {
  const char *p = name;
  while (p[0] != '\0' && !(p[0] == '/' && p[1] == '/'))
    ++p;
  while (*p != '\0') {
    while (*p == '/' || *p == ',')
      ++p;
    const char *f = flag;
    const char *q = p;
    while (*q != '\0' && *f != '\0' && internal::toupper(*q) == *f) {
      ++q;
      ++f;
    }
    if (*f == '\0' && (*q == '\0' || *q == '/' || *q == ','))
      return true;
    while (*p != '\0' && *p != '/' && *p != ',')
      ++p;
  }
  return false;
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
  NONE,       // The bytes were a byte order mark rather than a character.
  INCOMPLETE, // The input ran out part way through a character.
  INVALID,    // The bytes are not a character in this set.
  FULL,       // There is no room in the output.
};

// The units of UTF-16, UCS-2 and UTF-32, in either byte order.
LIBC_INLINE uint32_t get16(const unsigned char *p, bool big) {
  return big ? (uint32_t(p[0]) << 8) | p[1] : (uint32_t(p[1]) << 8) | p[0];
}

LIBC_INLINE uint32_t get32(const unsigned char *p, bool big) {
  uint32_t value = 0;
  for (size_t i = 0; i < 4; ++i)
    value = (value << 8) | p[big ? i : 3 - i];
  return value;
}

LIBC_INLINE void put16(unsigned char *p, uint32_t unit, bool big) {
  p[big ? 0 : 1] = static_cast<unsigned char>(unit >> 8);
  p[big ? 1 : 0] = static_cast<unsigned char>(unit & 0xFF);
}

LIBC_INLINE void put32(unsigned char *p, uint32_t unit, bool big) {
  for (size_t i = 0; i < 4; ++i)
    p[big ? i : 3 - i] = static_cast<unsigned char>(unit >> (24 - 8 * i));
}

// The value of a hex digit, or -1.
LIBC_INLINE int hex_digit(unsigned char c) {
  if (c >= '0' && c <= '9')
    return c - '0';
  if (c >= 'a' && c <= 'f')
    return c - 'a' + 10;
  if (c >= 'A' && c <= 'F')
    return c - 'A' + 10;
  return -1;
}

// Reads the hex digits of an escape. Returns how many of |count| digits were
// there before the input or the digits ran out.
LIBC_INLINE size_t read_hex(const unsigned char *in, size_t inleft,
                            size_t count, char32_t &value) {
  value = 0;
  size_t i = 0;
  for (; i < count && i < inleft; ++i) {
    int digit = hex_digit(in[i]);
    if (digit < 0)
      break;
    value = (value << 4) | static_cast<char32_t>(digit);
  }
  return i;
}

// Whether |encoding| may carry a byte order mark, and the unit it counts in.
LIBC_INLINE bool has_mark(Encoding encoding) {
  return encoding == Encoding::UTF16 || encoding == Encoding::UTF32 ||
         encoding == Encoding::UCS2_BOM;
}

// The form of |encoding| with its byte order settled.
LIBC_INLINE Encoding in_order(Encoding encoding, bool big) {
  switch (encoding) {
  case Encoding::UTF16:
    return big ? Encoding::UTF16BE : Encoding::UTF16LE;
  case Encoding::UTF32:
    return big ? Encoding::UTF32BE : Encoding::UTF32LE;
  case Encoding::UCS2_BOM:
    return big ? Encoding::UCS2BE : Encoding::UCS2LE;
  default:
    return encoding;
  }
}

// Reads one character of a set whose byte order is settled. |used| is how
// many bytes it took, or for input which is not a character, how many bytes
// are passed over to skip it.
LIBC_INLINE Status decode_as(Encoding from, const uint16_t *table,
                             const unsigned char *in, size_t inleft,
                             char32_t &out, size_t &used) {
  switch (from) {
  case Encoding::ASCII:
    used = 1;
    if (in[0] > 0x7F)
      return Status::INVALID;
    out = in[0];
    return Status::OK;

  case Encoding::SINGLE_BYTE: {
    used = 1;
    if (in[0] < 0x80) {
      out = in[0];
      return Status::OK;
    }
    uint16_t mapped = table[in[0] - 0x80];
    if (mapped == UNASSIGNED)
      return Status::INVALID;
    out = mapped;
    return Status::OK;
  }

  case Encoding::UTF8: {
    unsigned char lead = in[0];
    if (lead < 0x80) {
      out = lead;
      used = 1;
      return Status::OK;
    }
    // A continuation byte, 0xC0 or 0xC1, which could only start a character
    // written in more bytes than it needs, or anything from 0xF8 up cannot
    // start a character.
    size_t length = lead < 0xC2   ? 0
                    : lead < 0xE0 ? 2
                    : lead < 0xF0 ? 3
                    : lead < 0xF8 ? 4
                                  : 0;
    if (length == 0) {
      used = 1;
      return Status::INVALID;
    }
    // A byte which cannot continue the character makes it invalid at once,
    // even when the input ends before the character would have.
    size_t seen = 1;
    while (seen < length && seen < inleft && (in[seen] & 0xC0) == 0x80)
      ++seen;
    if (seen < length) {
      used = seen;
      return seen < inleft ? Status::INVALID : Status::INCOMPLETE;
    }
    char32_t value = lead & (0x7F >> length);
    for (size_t i = 1; i < length; ++i)
      value = (value << 6) | (in[i] & 0x3F);
    used = length;
    // A character written in more bytes than it needs, a surrogate, or one
    // past the end of Unicode is not a character.
    static constexpr char32_t LOWEST[5] = {0, 0, 0x80, 0x800, 0x10000};
    if (value < LOWEST[length] || value > 0x10FFFF ||
        (value >= 0xD800 && value <= 0xDFFF))
      return Status::INVALID;
    out = value;
    return Status::OK;
  }

  case Encoding::UTF16LE:
  case Encoding::UTF16BE: {
    if (inleft < 2)
      return Status::INCOMPLETE;
    const bool big = from == Encoding::UTF16BE;
    uint32_t first = get16(in, big);
    // A unit which does not begin a character is passed over on its own.
    used = 2;
    if (first < 0xD800 || first > 0xDFFF) {
      out = first;
      return Status::OK;
    }
    if (first >= 0xDC00)
      return Status::INVALID; // A low surrogate with no high one before it.
    if (inleft < 4)
      return Status::INCOMPLETE;
    uint32_t second = get16(in + 2, big);
    if (second < 0xDC00 || second > 0xDFFF)
      return Status::INVALID;
    out = 0x10000 + ((first - 0xD800) << 10) + (second - 0xDC00);
    used = 4;
    return Status::OK;
  }

  case Encoding::UCS2LE:
  case Encoding::UCS2BE: {
    if (inleft < 2)
      return Status::INCOMPLETE;
    used = 2;
    uint32_t value = get16(in, from == Encoding::UCS2BE);
    if (value >= 0xD800 && value <= 0xDFFF)
      return Status::INVALID;
    out = value;
    return Status::OK;
  }

  case Encoding::UTF32LE:
  case Encoding::UTF32BE: {
    if (inleft < 4)
      return Status::INCOMPLETE;
    uint32_t value = get32(in, from == Encoding::UTF32BE);
    used = 4;
    if (value > 0x10FFFF || (value >= 0xD800 && value <= 0xDFFF))
      return Status::INVALID;
    out = value;
    return Status::OK;
  }

  case Encoding::C99:
  case Encoding::JAVA: {
    const bool java = from == Encoding::JAVA;
    used = 1;
    if (in[0] != '\\') {
      // Java reads any other byte as the character of that value; C99 takes
      // only ASCII.
      if (in[0] >= 0x80 && !java)
        return Status::INVALID;
      out = in[0];
      return Status::OK;
    }
    // A backslash which does not begin an escape stands for itself.
    out = '\\';
    if (inleft < 2)
      return Status::INCOMPLETE;
    const size_t digits = in[1] == 'u' ? 4 : in[1] == 'U' && !java ? 8 : 0;
    if (digits == 0)
      return Status::OK;
    char32_t value;
    size_t got = read_hex(in + 2, inleft - 2, digits, value);
    if (got < digits)
      return got == inleft - 2 ? Status::INCOMPLETE : Status::OK;

    if (!java) {
      // C99 names no surrogate, nothing past Unicode, and nothing below U+00A0
      // but '$', '@' and '`'.
      if ((value >= 0xD800 && value <= 0xDFFF) || value > 0x10FFFF ||
          (value < 0xA0 && value != '$' && value != '@' && value != '`'))
        return Status::INVALID;
      out = value;
      used = 2 + digits;
      return Status::OK;
    }

    // In Java a surrogate is half a character: a high one needs the escape of
    // a low one right after it, and either on its own is left as text.
    if (value >= 0xDC00 && value <= 0xDFFF)
      return Status::OK;
    if (value < 0xD800 || value > 0xDBFF) {
      out = value;
      used = 6;
      return Status::OK;
    }
    const unsigned char *low = in + 6;
    const size_t lowleft = inleft - 6;
    if (lowleft == 0 || low[0] != '\\')
      return lowleft == 0 ? Status::INCOMPLETE : Status::OK;
    if (lowleft == 1 || low[1] != 'u')
      return lowleft == 1 ? Status::INCOMPLETE : Status::OK;
    char32_t second;
    got = read_hex(low + 2, lowleft - 2, 4, second);
    if (got < 4)
      return got == lowleft - 2 ? Status::INCOMPLETE : Status::OK;
    if (second < 0xDC00 || second > 0xDFFF)
      return Status::OK;
    out = 0x10000 + ((value - 0xD800) << 10) + (second - 0xDC00);
    used = 12;
    return Status::OK;
  }

  default:
    return Status::INVALID;
  }
}

// Reads one character, or the byte order mark ahead of the first one.
LIBC_INLINE Status decode(Conversion &conv, const unsigned char *in,
                          size_t inleft, char32_t &out, size_t &used) {
  if (inleft == 0)
    return Status::INCOMPLETE;

  if (has_mark(conv.from) && conv.read_mark) {
    const bool wide = conv.from == Encoding::UTF32;
    const size_t unit = wide ? 4 : 2;
    if (inleft < unit)
      return Status::INCOMPLETE;
    // Only the first unit can be a mark. Further in, FEFF is the character
    // U+FEFF.
    conv.read_mark = false;
    uint32_t first = wide ? get32(in, true) : get16(in, true);
    if (first == 0xFEFF || first == (wide ? 0xFFFE0000 : 0xFFFE)) {
      conv.read_big = first == 0xFEFF;
      used = unit;
      return Status::NONE;
    }
  }
  return decode_as(in_order(conv.from, conv.read_big), conv.from_table, in,
                   inleft, out, used);
}

// The fewest bytes a character takes in |encoding|. With less room than
// that, the output is full whatever the character turns out to be.
LIBC_INLINE size_t narrowest(Encoding encoding) {
  switch (encoding) {
  case Encoding::UTF16LE:
  case Encoding::UTF16BE:
  case Encoding::UTF16:
  case Encoding::UCS2LE:
  case Encoding::UCS2BE:
  case Encoding::UCS2_BOM:
    return 2;
  case Encoding::UTF32LE:
  case Encoding::UTF32BE:
  case Encoding::UTF32:
    return 4;
  default:
    return 1;
  }
}

// Writes one character of a set whose byte order is settled. |made| is how
// many bytes it took.
LIBC_INLINE Status encode_as(Encoding to, const uint16_t *table, char32_t cp,
                             unsigned char *out, size_t outleft, size_t &made) {
  switch (to) {
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
    // U+FFFD in the table marks a byte the set does not assign.
    if (cp > 0xFFFF || cp == UNASSIGNED)
      return Status::INVALID;
    // The table is small enough that a walk costs less than a second table
    // to invert it would.
    for (size_t i = 0; i < 128; ++i) {
      if (table[i] != cp)
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
    const bool big = to == Encoding::UTF16BE;
    if (cp < 0x10000) {
      if (outleft < 2)
        return Status::FULL;
      put16(out, cp, big);
      made = 2;
      return Status::OK;
    }
    if (outleft < 4)
      return Status::FULL;
    uint32_t rest = cp - 0x10000;
    put16(out, 0xD800 + (rest >> 10), big);
    put16(out + 2, 0xDC00 + (rest & 0x3FF), big);
    made = 4;
    return Status::OK;
  }

  case Encoding::UCS2LE:
  case Encoding::UCS2BE: {
    if (outleft < 2)
      return Status::FULL;
    if (cp > 0xFFFF)
      return Status::INVALID;
    put16(out, cp, to == Encoding::UCS2BE);
    made = 2;
    return Status::OK;
  }

  case Encoding::UTF32LE:
  case Encoding::UTF32BE: {
    if (outleft < 4)
      return Status::FULL;
    put32(out, cp, to == Encoding::UTF32BE);
    made = 4;
    return Status::OK;
  }

  case Encoding::C99:
  case Encoding::JAVA: {
    const bool java = to == Encoding::JAVA;
    // C99 cannot name a character below U+00A0 with an escape, so writes it
    // as it is. Java escapes everything past ASCII.
    if (cp < (java ? 0x80u : 0xA0u)) {
      if (outleft < 1)
        return Status::FULL;
      out[0] = static_cast<unsigned char>(cp);
      made = 1;
      return Status::OK;
    }
    static constexpr char HEX[] = "0123456789abcdef";
    auto escape = [](unsigned char *p, char kind, char32_t value,
                     size_t digits) {
      p[0] = '\\';
      p[1] = static_cast<unsigned char>(kind);
      for (size_t i = 0; i < digits; ++i)
        p[2 + i] = static_cast<unsigned char>(
            HEX[(value >> (4 * (digits - 1 - i))) & 0xF]);
    };
    if (cp < 0x10000) {
      if (outleft < 6)
        return Status::FULL;
      escape(out, 'u', cp, 4);
      made = 6;
    } else if (!java) {
      if (outleft < 10)
        return Status::FULL;
      escape(out, 'U', cp, 8);
      made = 10;
    } else {
      if (outleft < 12)
        return Status::FULL;
      char32_t rest = cp - 0x10000;
      escape(out, 'u', 0xD800 + (rest >> 10), 4);
      escape(out + 6, 'u', 0xDC00 + (rest & 0x3FF), 4);
      made = 12;
    }
    return Status::OK;
  }

  default:
    return Status::INVALID;
  }
}

// Writes one character, after the byte order mark if the output still owes
// one. |made| counts the mark as well, and a mark once written stays written
// even when the character after it cannot be.
LIBC_INLINE Status encode(Conversion &conv, char32_t cp, unsigned char *out,
                          size_t outleft, size_t &made) {
  made = 0;
  // Output with a mark is in the host's byte order, which the mark says.
  const bool big = !Endian::IS_LITTLE;
  if (has_mark(conv.to) && conv.write_mark) {
    const bool wide = conv.to == Encoding::UTF32;
    const size_t unit = wide ? 4 : 2;
    if (outleft < unit)
      return Status::FULL;
    if (wide)
      put32(out, 0xFEFF, big);
    else
      put16(out, 0xFEFF, big);
    conv.write_mark = false;
    made = unit;
    out += unit;
    outleft -= unit;
  }
  size_t wrote = 0;
  Status status =
      encode_as(in_order(conv.to, big), conv.to_table, cp, out, outleft, wrote);
  made += wrote;
  return status;
}

} // namespace iconv_internal
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_ICONV_CONVERSION_H
