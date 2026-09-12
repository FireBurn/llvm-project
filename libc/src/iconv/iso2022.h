//===-- The ISO-2022 sets of iconv ------------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// ISO-2022-JP, ISO-2022-JP-1, ISO-2022-JP-2, ISO-2022-KR and HZ, which switch
/// between sets with escape sequences and shifts.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_ICONV_ISO2022_H
#define LLVM_LIBC_SRC_ICONV_ISO2022_H

#include "hdr/stdint_proxy.h"
#include "hdr/types/char32_t.h"
#include "hdr/types/size_t.h"
#include "src/__support/macros/attributes.h"
#include "src/__support/macros/config.h"
#include "src/iconv/chinese.h"
#include "src/iconv/cjk_tables.h"
#include "src/iconv/code_table.h"
#include "src/iconv/japanese.h"
#include "src/iconv/korean.h"
#include "src/iconv/single_byte_tables.h"
#include "src/iconv/status.h"
#include "src/iconv/traditional_chinese.h"

namespace LIBC_NAMESPACE_DECL {
namespace iconv_internal {

// The sets a conversion can switch to.
enum class Iso2022Set : uint8_t {
  ASCII,
  JIS_ROMAN,
  JIS_KANA,
  JIS_X0208,
  JIS_X0212,
  GB2312,
  KS_C_5601,
  ISO8859_1,
  ISO8859_7,
  ISO_IR_165,
  CNS_PLANE_1,
  CNS_PLANE_2,
};

enum class Iso2022Language : uint8_t { NONE, JAPANESE, CHINESE, KOREAN };

// How many letters of a language tag count. Outside a tag, as many are
// taken as already read.
constexpr uint8_t TAG_LETTERS = 2;

// Where a conversion is. G2 is ASCII while nothing is designated to it.
struct Iso2022State {
  Iso2022Set read_g0;
  Iso2022Set read_g2;
  bool read_shifted;
  Iso2022Set write_g0;
  Iso2022Set write_g2;
  bool write_shifted;
  // Whether ISO-2022-KR's output has announced KS C 5601.
  bool write_announced;
  // For ISO-2022-JP-2: the language a tag last named, which decides between
  // the sets which have a Han character, and the letters of a tag so far.
  Iso2022Language language;
  uint8_t tag_length = TAG_LETTERS;
  char32_t tag_first;
  // For ISO-2022-CN-EXT: the planes of CNS 11643 designated for SS3, or 0.
  uint8_t read_g3;
  uint8_t write_g3;
  // For ISO-2022-CN: whether the last character written was shifted out.
  bool write_last_shifted;
};

struct Iso2022Escape {
  const char *bytes; // What follows the ESC.
  uint8_t length;
  Iso2022Set set;
};

constexpr uint8_t ESC = 0x1B;
constexpr uint8_t SO = 0x0E;
constexpr uint8_t SI = 0x0F;

// ISO-2022-JP's escape sequences come first; ISO-2022-JP-2 has them all.
constexpr size_t ISO2022_JP_ESCAPE_COUNT = 4;
constexpr Iso2022Escape ISO2022_JP_ESCAPES[] = {
    {"(B", 2, Iso2022Set::ASCII},      {"(J", 2, Iso2022Set::JIS_ROMAN},
    {"$B", 2, Iso2022Set::JIS_X0208},  {"$@", 2, Iso2022Set::JIS_X0208},
    {"$A", 2, Iso2022Set::GB2312},     {"$(C", 3, Iso2022Set::KS_C_5601},
    {"$(D", 3, Iso2022Set::JIS_X0212}, {"(I", 2, Iso2022Set::JIS_KANA},
    {".A", 2, Iso2022Set::ISO8859_1},  {".F", 2, Iso2022Set::ISO8859_7},
};

LIBC_INLINE bool is_g2(Iso2022Set set) {
  return set == Iso2022Set::ISO8859_1 || set == Iso2022Set::ISO8859_7;
}

// Whether a byte can begin a character of a set of two byte characters. glibc
// only waits for the second byte after one from the first row with characters
// to the last, except in JIS X 0208 and KS C 5601, where it always waits.
LIBC_INLINE bool iso2022_may_begin(Iso2022Set set, unsigned byte) {
  switch (set) {
  case Iso2022Set::JIS_X0212:
    return byte >= 0x22 && byte <= 0x6D;
  case Iso2022Set::GB2312:
    return byte <= 0x77;
  default:
    return true;
  }
}

LIBC_INLINE char32_t iso2022_character(Iso2022Set set, unsigned first,
                                       unsigned second) {
  switch (set) {
  case Iso2022Set::JIS_X0208:
    return look_up(JIS_X0208, first, second);
  case Iso2022Set::JIS_X0212:
    return look_up(JIS_X0212, first, second);
  case Iso2022Set::GB2312:
    return gb2312_character(first, second);
  case Iso2022Set::KS_C_5601:
    return look_up(KS_X_1001, first, second);
  default:
    return 0;
  }
}

// G2's character for a byte after ESC N, which glibc reads as if its high bit
// were set, whatever the byte is.
LIBC_INLINE char32_t iso2022_g2_character(Iso2022Set set, unsigned byte) {
  if (set == Iso2022Set::ISO8859_1)
    return 0x80 | byte;
  // For ISO-8859-7 it takes a byte from 0x20 to 0x7F only.
  if (byte < 0x20 || byte >= 0x80)
    return 0;
  const uint16_t cp = ISO8859_7_HIGH[byte];
  return cp == UNASSIGNED ? 0 : cp;
}

LIBC_INLINE Status read_iso2022_jp(Iso2022State &state, bool jp2,
                                   const unsigned char *in, size_t inleft,
                                   char32_t &out, size_t &used) {
  const unsigned byte = in[0];
  used = 1;
  if (byte == ESC) {
    // glibc waits for three bytes before it looks at an escape sequence.
    if (inleft < 3 || (jp2 && inleft < 4 && in[1] == '$' && in[2] == '('))
      return Status::INCOMPLETE;
    if (jp2 && in[1] == 'N') {
      if (state.read_g2 == Iso2022Set::ASCII)
        return Status::INVALID;
      char32_t cp = iso2022_g2_character(state.read_g2, in[2]);
      if (cp == 0)
        return Status::INVALID;
      used = 3;
      out = cp;
      return Status::OK;
    }
    const size_t count =
        jp2 ? sizeof(ISO2022_JP_ESCAPES) / sizeof(ISO2022_JP_ESCAPES[0])
            : ISO2022_JP_ESCAPE_COUNT;
    for (size_t i = 0; i < count; ++i) {
      const Iso2022Escape &escape = ISO2022_JP_ESCAPES[i];
      if (1u + escape.length > inleft)
        continue;
      size_t same = 0;
      while (same < escape.length &&
             in[1 + same] == static_cast<unsigned char>(escape.bytes[same]))
        ++same;
      if (same < escape.length)
        continue;
      if (is_g2(escape.set))
        state.read_g2 = escape.set;
      else
        state.read_g0 = escape.set;
      used = 1 + escape.length;
      return Status::NONE;
    }
    // An escape sequence it does not know is read as the characters it is.
    out = byte;
    return Status::OK;
  }

  switch (state.read_g0) {
  case Iso2022Set::ASCII:
    if (byte >= 0x80)
      return Status::INVALID;
    out = byte;
    return Status::OK;
  case Iso2022Set::JIS_ROMAN:
    if (byte >= 0x80)
      return Status::INVALID;
    out = byte == 0x5C ? 0xA5 : byte == 0x7E ? 0x203E : byte;
    return Status::OK;
  case Iso2022Set::JIS_KANA:
    if (byte < 0x21 || byte == 0x7F) {
      out = byte;
      return Status::OK;
    }
    if (byte > 0x5F)
      return Status::INVALID;
    out = 0xFF61 + (byte - 0x21);
    return Status::OK;
  default:
    break;
  }
  // In a set of two byte characters, a control byte is still read by itself.
  if (byte < 0x21 || byte == 0x7F) {
    out = byte;
    return Status::OK;
  }
  // glibc skips a pair which is not a character by its first byte alone.
  if (byte >= 0x80)
    return Status::INVALID;
  if (inleft < 2)
    return iso2022_may_begin(state.read_g0, byte) ? Status::INCOMPLETE
                                                  : Status::INVALID;
  const unsigned second = in[1];
  if (second < 0x21 || second >= 0x7F)
    return Status::INVALID;
  char32_t cp = iso2022_character(state.read_g0, byte, second);
  if (cp == 0)
    return Status::INVALID;
  out = cp;
  used = 2;
  return Status::OK;
}

// The code of |cp| in |set|, if the set has it.
LIBC_INLINE bool iso2022_code(Iso2022Set set, char32_t cp, uint16_t &code) {
  switch (set) {
  case Iso2022Set::ASCII:
    code = static_cast<uint16_t>(cp);
    return cp < 0x80;
  case Iso2022Set::JIS_ROMAN:
    code = cp == 0xA5 ? 0x5C : cp == 0x203E ? 0x7E : static_cast<uint16_t>(cp);
    return cp == 0xA5 || cp == 0x203E ||
           (cp < 0x80 && cp != 0x5C && cp != 0x7E);
  case Iso2022Set::JIS_KANA:
    code = static_cast<uint16_t>(cp - 0xFF61 + 0x21);
    return cp >= 0xFF61 && cp <= 0xFF9F;
  case Iso2022Set::JIS_X0208:
    code = find_code(JIS_X0208, cp);
    return code != 0;
  case Iso2022Set::JIS_X0212:
    // glibc also writes ASCII's tilde as the fullwidth one while JIS X 0212 is
    // in use.
    code = cp == 0x7E ? 0x2237 : find_code(JIS_X0212, cp);
    return code != 0;
  case Iso2022Set::GB2312:
    code = gb2312_code(cp);
    return code != 0;
  case Iso2022Set::KS_C_5601:
    code = find_code(KS_X_1001, cp);
    return code != 0;
  case Iso2022Set::ISO_IR_165:
  case Iso2022Set::CNS_PLANE_1:
  case Iso2022Set::CNS_PLANE_2:
    return false;
  case Iso2022Set::ISO8859_1:
    code = static_cast<uint16_t>(cp - 0x80);
    return cp >= 0xA0 && cp <= 0xFF;
  case Iso2022Set::ISO8859_7:
    for (unsigned i = 0x20; i < 0x80; ++i) {
      if (ISO8859_7_HIGH[i] == cp) {
        code = static_cast<uint16_t>(i);
        return true;
      }
    }
    return false;
  }
  return false;
}

LIBC_INLINE const Iso2022Escape &iso2022_escape(Iso2022Set set) {
  size_t i = 0;
  while (ISO2022_JP_ESCAPES[i].set != set)
    ++i;
  return ISO2022_JP_ESCAPES[i];
}

// Language tags, from U+E0000, name a language for what follows.
LIBC_INLINE void read_tag(Iso2022State &state, char32_t cp) {
  if (cp == 0xE0001) {
    state.language = Iso2022Language::NONE;
    state.tag_length = 0;
    return;
  }
  if (cp == 0xE007F) {
    state.language = Iso2022Language::NONE;
    return;
  }
  // Letters only count after U+E0001, and only the first two of them.
  if (cp < 0xE0061 || cp > 0xE007A || state.tag_length >= TAG_LETTERS)
    return;
  if (state.tag_length++ == 0) {
    state.tag_first = cp;
    return;
  }
  const char32_t first = state.tag_first - 0xE0000;
  const char32_t second = cp - 0xE0000;
  if (first == 'j' && second == 'a')
    state.language = Iso2022Language::JAPANESE;
  else if (first == 'z' && second == 'h')
    state.language = Iso2022Language::CHINESE;
  else if (first == 'k' && second == 'o')
    state.language = Iso2022Language::KOREAN;
  else
    state.language = Iso2022Language::NONE;
}

// Whether a language named by a tag lets the set in use be kept.
LIBC_INLINE bool suits(Iso2022Language language, Iso2022Set set) {
  switch (language) {
  case Iso2022Language::JAPANESE:
    return set != Iso2022Set::GB2312 && set != Iso2022Set::KS_C_5601;
  case Iso2022Language::CHINESE:
    return set == Iso2022Set::ASCII || set == Iso2022Set::GB2312;
  case Iso2022Language::KOREAN:
    return set == Iso2022Set::ASCII || set == Iso2022Set::KS_C_5601;
  default:
    return true;
  }
}

LIBC_INLINE Status write_iso2022_jp(Iso2022State &state, bool jp2, char32_t cp,
                                    unsigned char *out, size_t outleft,
                                    size_t &made) {
  made = 0;
  if (cp >= 0xE0000 && cp <= 0xE007F) {
    if (!jp2)
      return Status::INVALID;
    read_tag(state, cp);
    return Status::OK;
  }

  // The set in use is kept while it has the character and suits the language
  // a tag named, except for a space or a control character. So is a
  // designated G2, while no language is named. Otherwise the first set in
  // order which has it is used.
  uint16_t code = 0;
  Iso2022Set set = Iso2022Set::ASCII;
  bool found = false;
  const bool newline = cp == '\n';
  if (suits(state.language, state.write_g0) && cp > 0x20 &&
      iso2022_code(state.write_g0, cp, code)) {
    set = state.write_g0;
    found = true;
  } else if (jp2 && state.language == Iso2022Language::NONE &&
             state.write_g2 != Iso2022Set::ASCII &&
             iso2022_code(state.write_g2, cp, code)) {
    set = state.write_g2;
    found = true;
  }
  if (!found) {
    // A Chinese or Korean tag puts its own set and ISO-8859's first.
    using S = Iso2022Set;
    static constexpr S USUAL[] = {S::ASCII,     S::JIS_ROMAN, S::JIS_X0208,
                                  S::JIS_X0212, S::GB2312,    S::ISO8859_1,
                                  S::ISO8859_7, S::KS_C_5601, S::JIS_KANA};
    static constexpr S CHINESE[] = {S::ASCII,     S::GB2312,    S::ISO8859_1,
                                    S::ISO8859_7, S::JIS_ROMAN, S::JIS_X0208,
                                    S::JIS_X0212, S::KS_C_5601, S::JIS_KANA};
    static constexpr S KOREAN[] = {S::ASCII,     S::KS_C_5601, S::ISO8859_1,
                                   S::ISO8859_7, S::JIS_ROMAN, S::JIS_X0208,
                                   S::JIS_X0212, S::GB2312,    S::JIS_KANA};
    const S *order = USUAL;
    if (jp2 && state.language == Iso2022Language::CHINESE)
      order = CHINESE;
    else if (jp2 && state.language == Iso2022Language::KOREAN)
      order = KOREAN;
    // ISO-2022-JP has only ASCII, JIS X 0201 Roman and JIS X 0208.
    const size_t count = jp2 ? sizeof(USUAL) / sizeof(USUAL[0]) : 3;
    for (size_t i = 0; i < count && !found; ++i) {
      if (iso2022_code(order[i], cp, code)) {
        set = order[i];
        found = true;
      }
    }
  }
  if (!found)
    return Status::INVALID;

  unsigned char bytes[8];
  size_t length = 0;
  if (is_g2(set)) {
    if (state.write_g2 != set) {
      const Iso2022Escape &escape = iso2022_escape(set);
      bytes[length++] = ESC;
      for (size_t i = 0; i < escape.length; ++i)
        bytes[length++] = static_cast<unsigned char>(escape.bytes[i]);
    }
    bytes[length++] = ESC;
    bytes[length++] = 'N';
    bytes[length++] = static_cast<unsigned char>(code);
  } else {
    if (state.write_g0 != set) {
      const Iso2022Escape &escape = iso2022_escape(set);
      bytes[length++] = ESC;
      for (size_t i = 0; i < escape.length; ++i)
        bytes[length++] = static_cast<unsigned char>(escape.bytes[i]);
    }
    if (code > 0xFF || set == Iso2022Set::JIS_X0208 ||
        set == Iso2022Set::JIS_X0212 || set == Iso2022Set::GB2312 ||
        set == Iso2022Set::KS_C_5601)
      bytes[length++] = static_cast<unsigned char>(code >> 8);
    bytes[length++] = static_cast<unsigned char>(code & 0xFF);
  }
  if (outleft < length)
    return Status::FULL;
  for (size_t i = 0; i < length; ++i)
    out[i] = bytes[i];
  made = length;
  if (is_g2(set))
    state.write_g2 = set;
  else
    state.write_g0 = set;
  // After a new line, G2 is designated again before it is used.
  if (newline)
    state.write_g2 = Iso2022Set::ASCII;
  return Status::OK;
}

// ISO-2022-KR: ASCII, and KS C 5601 between SO and SI. glibc reads it with or
// without the announcement ESC $ ) C, and reads any other escape sequence as
// the characters it is.
LIBC_INLINE Status read_iso2022_kr(Iso2022State &state, const unsigned char *in,
                                   size_t inleft, char32_t &out, size_t &used) {
  const unsigned byte = in[0];
  used = 1;
  if (byte == ESC) {
    static constexpr char ANNOUNCEMENT[] = "\x1b$)C";
    size_t same = 1;
    while (same < 4 && same < inleft &&
           in[same] == static_cast<unsigned char>(ANNOUNCEMENT[same]))
      ++same;
    if (same == 4) {
      used = 4;
      return Status::NONE;
    }
    if (same == inleft)
      return Status::INCOMPLETE;
    // Shifted out, it is not a character either.
    if (state.read_shifted)
      return Status::INVALID;
    out = byte;
    return Status::OK;
  }
  if (byte == SO || byte == SI) {
    state.read_shifted = byte == SO;
    return Status::NONE;
  }
  if (byte >= 0x80)
    return Status::INVALID;
  if (!state.read_shifted) {
    out = byte;
    return Status::OK;
  }
  if (inleft < 2)
    return Status::INCOMPLETE;
  const unsigned second = in[1];
  if (second < 0x21 || second >= 0x7F)
    return Status::INVALID;
  char32_t cp = look_up(KS_X_1001, byte, second);
  if (cp == 0)
    return Status::INVALID;
  out = cp;
  used = 2;
  return Status::OK;
}

LIBC_INLINE Status write_iso2022_kr(Iso2022State &state, char32_t cp,
                                    unsigned char *out, size_t outleft,
                                    size_t &made) {
  made = 0;
  // glibc announces KS C 5601 at the start of the output, before it knows
  // whether any is needed. The announcement stays written, whatever becomes
  // of the character after it.
  if (!state.write_announced) {
    if (outleft < 4)
      return Status::FULL;
    out[0] = ESC;
    out[1] = '$';
    out[2] = ')';
    out[3] = 'C';
    made = 4;
    out += 4;
    outleft -= 4;
    state.write_announced = true;
  }
  // glibc takes language tags and writes nothing for them.
  if (cp >= 0xE0000 && cp <= 0xE007F)
    return Status::OK;
  unsigned char bytes[3];
  size_t length = 0;
  bool shifted = state.write_shifted;
  if (cp < 0x80) {
    if (shifted)
      bytes[length++] = SI;
    bytes[length++] = static_cast<unsigned char>(cp);
    shifted = false;
  } else {
    const uint16_t code = find_code(KS_X_1001, cp);
    if (code == 0)
      return Status::INVALID;
    if (!shifted)
      bytes[length++] = SO;
    bytes[length++] = static_cast<unsigned char>(code >> 8);
    bytes[length++] = static_cast<unsigned char>(code & 0xFF);
    shifted = true;
  }
  if (outleft < length)
    return Status::FULL;
  for (size_t i = 0; i < length; ++i)
    out[i] = bytes[i];
  made += length;
  state.write_shifted = shifted;
  return Status::OK;
}

// What the output owes before it is back in its initial state.
LIBC_INLINE Status unshift_iso2022(Iso2022State &state, bool korean,
                                   unsigned char *out, size_t outleft,
                                   size_t &made) {
  unsigned char bytes[5];
  size_t length = 0;
  if (korean) {
    if (!state.write_announced) {
      bytes[length++] = ESC;
      bytes[length++] = '$';
      bytes[length++] = ')';
      bytes[length++] = 'C';
    }
    if (state.write_shifted)
      bytes[length++] = SI;
  } else if (state.write_g0 != Iso2022Set::ASCII) {
    bytes[length++] = ESC;
    bytes[length++] = '(';
    bytes[length++] = 'B';
  }
  if (outleft < length)
    return Status::FULL;
  for (size_t i = 0; i < length; ++i)
    out[i] = bytes[i];
  made = length;
  return Status::OK;
}

// ISO-2022-JP-1 is ISO-2022-JP with JIS X 0212 as well. ISO-2022-JP-MS adds
// half-width katakana and what CP932 has beyond JIS X 0208: NEC's row 13 in row
// 0x2D, the NEC-selected IBM extensions in rows 0x79 to 0x7C, the rest of IBM's
// in rows 0x73 and 0x74 of JIS X 0212, and private use characters in the rows
// from 0x75 of both. glibc has neither, so they are read and written as GNU
// libiconv does: an escape sequence it does not know is not a character, a byte
// in a set of two byte characters waits for the next, a pair which is not a
// character is skipped by its first byte, and writing goes back to ASCII for
// every character ASCII has. In ISO-2022-JP-MS a shift out goes from JIS X 0201
// Roman to katakana and a shift in comes back, and otherwise they do nothing.
constexpr Iso2022Escape ISO2022_JP1_ESCAPES[] = {
    {"(B", 2, Iso2022Set::ASCII},
    {"(J", 2, Iso2022Set::JIS_ROMAN},
    {"$B", 2, Iso2022Set::JIS_X0208},
    {"$@", 2, Iso2022Set::JIS_X0208},
    {"$(D", 3, Iso2022Set::JIS_X0212},
    // ISO-2022-JP-MS only.
    {"(I", 2, Iso2022Set::JIS_KANA},
};
constexpr size_t ISO2022_JP1_ESCAPE_COUNT = 5;

// The character of a JIS X 0208 or JIS X 0212 code in ISO-2022-JP-MS, or 0.
LIBC_INLINE char32_t iso2022_jp_ms_character(Iso2022Set set, unsigned row,
                                             unsigned column) {
  if (set == Iso2022Set::JIS_X0212) {
    if (char32_t cp = look_up(JIS_X0212, row, column))
      return cp;
    if (char32_t cp = look_up(ISO2022_JP_MS_IBM, row, column))
      return cp;
    return row >= 0x75 ? 0xE3AC + (row - 0x75) * 94 + (column - 0x21) : 0;
  }
  if (char32_t cp = look_up(JIS_X0208, row, column))
    return cp;
  const uint16_t code =
      shift_jis_from_jis(static_cast<uint16_t>(row << 8 | column));
  if (row == 0x2D) {
    // GNU libiconv reads 0x2D60 as U+301E where CP932 has U+301D, and not the
    // symbols row 13 repeats from JIS X 0208.
    if (column == 0x60)
      return 0x301E;
    char32_t cp = look_up(CP932_NEC_ROW_13, code >> 8, code & 0xFF);
    return find_code(JIS_X0208, cp) ? 0 : cp;
  }
  if (row >= 0x79 && row <= 0x7C)
    if (char32_t cp = look_up(CP932_NEC_SELECTED_IBM, code >> 8, code & 0xFF))
      return cp;
  return row >= 0x75 ? 0xE000 + (row - 0x75) * 94 + (column - 0x21) : 0;
}

// The set and code ISO-2022-JP-MS writes a character beyond ASCII and
// katakana as, trying JIS X 0208, row 13, JIS X 0212 and then NEC's selection.
LIBC_INLINE bool iso2022_jp_ms_code(char32_t cp, Iso2022Set &set,
                                    uint16_t &code) {
  set = Iso2022Set::JIS_X0208;
  code = cp == 0x301E ? 0x2D60 : find_code(JIS_X0208, cp);
  if (code)
    return true;
  if (uint16_t shift_jis = find_code(CP932_NEC_ROW_13, cp)) {
    code = jis_from_shift_jis(shift_jis >> 8, shift_jis & 0xFF);
    return true;
  }
  set = Iso2022Set::JIS_X0212;
  code = find_code(JIS_X0212, cp);
  if (!code)
    code = find_code(ISO2022_JP_MS_IBM, cp);
  if (code)
    return true;
  set = Iso2022Set::JIS_X0208;
  if (uint16_t shift_jis = find_code(CP932_NEC_SELECTED_IBM, cp)) {
    code = jis_from_shift_jis(shift_jis >> 8, shift_jis & 0xFF);
    return true;
  }
  if (cp < 0xE000 || cp >= 0xE000 + 20 * 94)
    return false;
  unsigned index = cp - 0xE000;
  if (index >= 10 * 94) {
    set = Iso2022Set::JIS_X0212;
    index -= 10 * 94;
  }
  code = static_cast<uint16_t>((0x75 + index / 94) << 8 | (0x21 + index % 94));
  return true;
}

LIBC_INLINE Status read_iso2022_jp1(Iso2022State &state, bool ms,
                                    const unsigned char *in, size_t inleft,
                                    char32_t &out, size_t &used) {
  const unsigned byte = in[0];
  used = 1;
  if (byte == ESC) {
    if (inleft < 3)
      return Status::INCOMPLETE;
    bool waiting = false;
    const size_t count = ISO2022_JP1_ESCAPE_COUNT + (ms ? 1 : 0);
    for (size_t e = 0; e < count; ++e) {
      const Iso2022Escape &escape = ISO2022_JP1_ESCAPES[e];
      const size_t have =
          inleft - 1 < escape.length ? inleft - 1 : escape.length;
      size_t same = 0;
      while (same < have &&
             in[1 + same] == static_cast<unsigned char>(escape.bytes[same]))
        ++same;
      if (same < have)
        continue;
      if (have < escape.length) {
        waiting = true;
        continue;
      }
      state.read_g0 = escape.set;
      used = 1 + escape.length;
      return Status::NONE;
    }
    return waiting ? Status::INCOMPLETE : Status::INVALID;
  }
  if (ms && (byte == SO || byte == SI)) {
    if (byte == SO && state.read_g0 == Iso2022Set::JIS_ROMAN)
      state.read_g0 = Iso2022Set::JIS_KANA;
    else if (byte == SI && state.read_g0 == Iso2022Set::JIS_KANA)
      state.read_g0 = Iso2022Set::JIS_ROMAN;
    return Status::NONE;
  }
  if (state.read_g0 == Iso2022Set::ASCII ||
      state.read_g0 == Iso2022Set::JIS_ROMAN) {
    if (byte >= 0x80)
      return Status::INVALID;
    out = state.read_g0 == Iso2022Set::ASCII ? byte
          : byte == 0x5C                     ? 0xA5
          : byte == 0x7E                     ? 0x203E
                                             : byte;
    return Status::OK;
  }
  if (state.read_g0 == Iso2022Set::JIS_KANA) {
    if (byte < 0x21 || byte > 0x5F)
      return Status::INVALID;
    out = 0xFF40 + byte;
    return Status::OK;
  }
  // In a two byte set every byte waits for the next before it is judged.
  if (inleft < 2)
    return Status::INCOMPLETE;
  if (byte < 0x21 || byte >= 0x7F)
    return Status::INVALID;
  const unsigned second = in[1];
  if (second < 0x21 || second >= 0x7F)
    return Status::INVALID;
  char32_t cp = ms ? iso2022_jp_ms_character(state.read_g0, byte, second)
                   : iso2022_character(state.read_g0, byte, second);
  if (cp == 0)
    return Status::INVALID;
  used = 2;
  out = cp;
  return Status::OK;
}

// Writes |code| in |set|, switching G0 to it first.
LIBC_INLINE Status write_in_g0(Iso2022State &state, Iso2022Set set,
                               uint16_t code, unsigned char *out,
                               size_t outleft, size_t &made) {
  unsigned char bytes[6];
  size_t length = 0;
  if (state.write_g0 != set) {
    const Iso2022Escape &escape = iso2022_escape(set);
    bytes[length++] = ESC;
    for (size_t i = 0; i < escape.length; ++i)
      bytes[length++] = static_cast<unsigned char>(escape.bytes[i]);
  }
  if (set == Iso2022Set::JIS_X0208 || set == Iso2022Set::JIS_X0212)
    bytes[length++] = static_cast<unsigned char>(code >> 8);
  bytes[length++] = static_cast<unsigned char>(code & 0xFF);
  if (outleft < length)
    return Status::FULL;
  for (size_t i = 0; i < length; ++i)
    out[i] = bytes[i];
  made = length;
  state.write_g0 = set;
  return Status::OK;
}

LIBC_INLINE Status write_iso2022_jp1(Iso2022State &state, char32_t cp,
                                     unsigned char *out, size_t outleft,
                                     size_t &made) {
  made = 0;
  // GNU libiconv takes language tags and writes nothing for them.
  if (cp >= 0xE0000 && cp <= 0xE007F)
    return Status::OK;
  static constexpr Iso2022Set ORDER[] = {
      Iso2022Set::ASCII, Iso2022Set::JIS_ROMAN, Iso2022Set::JIS_X0208,
      Iso2022Set::JIS_X0212};
  uint16_t code = 0;
  for (const Iso2022Set set : ORDER)
    if (iso2022_code(set, cp, code))
      return write_in_g0(state, set, code, out, outleft, made);
  return Status::INVALID;
}

LIBC_INLINE Status write_iso2022_jp_ms(Iso2022State &state, char32_t cp,
                                       unsigned char *out, size_t outleft,
                                       size_t &made) {
  made = 0;
  if (cp >= 0xE0000 && cp <= 0xE007F)
    return Status::OK;
  Iso2022Set set = Iso2022Set::ASCII;
  uint16_t code = static_cast<uint16_t>(cp);
  if (cp >= 0xFF61 && cp <= 0xFF9F) {
    set = Iso2022Set::JIS_KANA;
    code = static_cast<uint16_t>(cp - 0xFF40);
  } else if (cp >= 0x80 && !iso2022_jp_ms_code(cp, set, code)) {
    return Status::INVALID;
  }
  return write_in_g0(state, set, code, out, outleft, made);
}

// HZ: ASCII, and GB 2312 between "~{" and "~}". "~~" is a tilde, and a "~" at
// the end of a line joins it to the next. glibc does not have it, so it is
// read and written as GNU libiconv does, which also reads a byte from 0x80 as
// the character of that value, and writes a tilde as it is.
LIBC_INLINE Status read_hz(Iso2022State &state, const unsigned char *in,
                           size_t inleft, char32_t &out, size_t &used) {
  const unsigned byte = in[0];
  used = 1;
  if (byte == '~') {
    if (inleft < 2)
      return Status::INCOMPLETE;
    const unsigned next = in[1];
    if (!state.read_shifted && (next == '~' || next == '\n')) {
      used = 2;
      if (next == '\n')
        return Status::NONE;
      out = '~';
      return Status::OK;
    }
    if (next == (state.read_shifted ? '}' : '{')) {
      state.read_shifted = !state.read_shifted;
      used = 2;
      return Status::NONE;
    }
    return Status::INVALID;
  }
  if (!state.read_shifted) {
    out = byte;
    return Status::OK;
  }
  if (inleft < 2)
    return Status::INCOMPLETE;
  // A pair which is not a character skips one byte.
  char32_t cp = gb2312_character(byte, in[1]);
  if (byte < 0x21 || byte > 0x7E || cp == 0)
    return Status::INVALID;
  used = 2;
  out = cp;
  return Status::OK;
}

LIBC_INLINE Status write_hz(Iso2022State &state, char32_t cp,
                            unsigned char *out, size_t outleft, size_t &made) {
  made = 0;
  if (cp >= 0xE0000 && cp <= 0xE007F)
    return Status::OK;
  unsigned char bytes[4];
  size_t length = 0;
  bool shifted = state.write_shifted;
  if (cp < 0x80) {
    if (shifted) {
      bytes[length++] = '~';
      bytes[length++] = '}';
    }
    bytes[length++] = static_cast<unsigned char>(cp);
    shifted = false;
  } else {
    const uint16_t code = gb2312_code(cp);
    if (code == 0)
      return Status::INVALID;
    if (!shifted) {
      bytes[length++] = '~';
      bytes[length++] = '{';
    }
    bytes[length++] = static_cast<unsigned char>(code >> 8);
    bytes[length++] = static_cast<unsigned char>(code & 0xFF);
    shifted = true;
  }
  if (outleft < length)
    return Status::FULL;
  for (size_t i = 0; i < length; ++i)
    out[i] = bytes[i];
  made = length;
  state.write_shifted = shifted;
  return Status::OK;
}

LIBC_INLINE Status unshift_hz(Iso2022State &state, unsigned char *out,
                              size_t outleft, size_t &made) {
  if (!state.write_shifted)
    return Status::OK;
  if (outleft < 2)
    return Status::FULL;
  out[0] = '~';
  out[1] = '}';
  made = 2;
  return Status::OK;
}

// ISO-2022-CN: ASCII, and after SO the set the last ESC $ ) designated, GB 2312
// or plane 1 of CNS 11643. ESC N takes the next two bytes from plane 2.
// ISO-2022-CN-EXT adds ISO-IR-165 for SO, and ESC O for the plane from 3 to 7
// the last ESC $ + designated. They are read and written as glibc does, with
// these exceptions. glibc ignores a designation made while shifted out, which
// it writes itself, so that takes effect at once. After a character from plane
// 2 or later, glibc writes the next such character without its ESC N or ESC O,
// and shifts in or out as if it had shifted out for it; each has its own here,
// and only SO shifts out. Characters of planes 3 to 7 beyond the Basic
// Multilingual Plane are written, which glibc does not.
struct Iso2022CnEscape {
  char bytes[2];
  Iso2022Set set;
  uint8_t plane;
};

// The designations after ESC $, and last the ones ISO-2022-CN-EXT adds.
constexpr Iso2022CnEscape ISO2022_CN_ESCAPES[] = {
    {{')', 'A'}, Iso2022Set::GB2312, 0},
    {{')', 'G'}, Iso2022Set::CNS_PLANE_1, 0},
    {{'*', 'H'}, Iso2022Set::CNS_PLANE_2, 2},
    {{')', 'E'}, Iso2022Set::ISO_IR_165, 0},
    {{'+', 'I'}, Iso2022Set::ASCII, 3},
    {{'+', 'J'}, Iso2022Set::ASCII, 4},
    {{'+', 'K'}, Iso2022Set::ASCII, 5},
    {{'+', 'L'}, Iso2022Set::ASCII, 6},
    {{'+', 'M'}, Iso2022Set::ASCII, 7},
};
constexpr size_t ISO2022_CN_ESCAPE_COUNT = 3;

// The character of a code shifted out to |set|, or 0. Before any designation,
// glibc reads GB 2312.
LIBC_INLINE char32_t iso2022_cn_character(Iso2022Set set, unsigned row,
                                          unsigned column) {
  if (set == Iso2022Set::CNS_PLANE_1)
    return look_up(CNS_11643[0], row, column);
  if (set == Iso2022Set::ISO_IR_165)
    return iso_ir_165_character(row, column);
  return gb2312_character(row, column);
}

LIBC_INLINE uint16_t iso2022_cn_code(Iso2022Set set, char32_t cp) {
  if (set == Iso2022Set::GB2312)
    return gb2312_code(cp);
  if (set == Iso2022Set::ISO_IR_165)
    return iso_ir_165_code(cp);
  if (set == Iso2022Set::CNS_PLANE_1)
    return find_code(CNS_11643[0], cp);
  return 0;
}

LIBC_INLINE Status read_iso2022_cn(Iso2022State &state, bool ext,
                                   const unsigned char *in, size_t inleft,
                                   char32_t &out, size_t &used) {
  const unsigned byte = in[0];
  used = 1;
  if (byte == ESC) {
    if (inleft < 2)
      return Status::INCOMPLETE;
    if (in[1] == 'N' || (ext && in[1] == 'O')) {
      if (inleft < 4)
        return Status::INCOMPLETE;
      const WideTable *table = cns_plane(in[1] == 'N' ? 2 : state.read_g3);
      const char32_t cp = table ? look_up(*table, in[2], in[3]) : 0;
      // glibc skips the single shift of a code which is not a character, and
      // in ISO-2022-CN-EXT the code too.
      if (cp == 0) {
        used = ext ? 4 : 2;
        return Status::INVALID;
      }
      used = 4;
      out = cp;
      return Status::OK;
    }
    const size_t count =
        ext ? sizeof(ISO2022_CN_ESCAPES) / sizeof(ISO2022_CN_ESCAPES[0])
            : ISO2022_CN_ESCAPE_COUNT;
    for (size_t e = 0; in[1] == '$' && e < count; ++e) {
      const Iso2022CnEscape &escape = ISO2022_CN_ESCAPES[e];
      if (inleft < 4) {
        if (inleft == 2 || in[2] == static_cast<unsigned char>(escape.bytes[0]))
          return Status::INCOMPLETE;
        continue;
      }
      if (in[2] != static_cast<unsigned char>(escape.bytes[0]) ||
          in[3] != static_cast<unsigned char>(escape.bytes[1]))
        continue;
      if (escape.plane >= 3)
        state.read_g3 = escape.plane;
      else if (escape.plane == 0)
        state.read_g0 = escape.set;
      used = 4;
      return Status::NONE;
    }
    // As in glibc, an escape sequence it does not know is read as the
    // characters it is, unless shifted out.
    if (state.read_shifted)
      return Status::INVALID;
    out = byte;
    return Status::OK;
  }
  // glibc reads a shift out with nothing designated for it only in
  // ISO-2022-CN.
  if (byte == SO && ext && state.read_g0 == Iso2022Set::ASCII)
    return Status::INVALID;
  if (byte == SO || byte == SI) {
    state.read_shifted = byte == SO;
    return Status::NONE;
  }
  if (!state.read_shifted) {
    // glibc takes DEL for a character only in ISO-2022-CN-EXT.
    if (byte >= 0x80 || (byte == 0x7F && !ext))
      return Status::INVALID;
    out = byte;
    return Status::OK;
  }
  // Shifted out, glibc waits for a second byte after any byte below 0x80 in
  // ISO-2022-CN-EXT, and after one from 0x21 to 0x7E in ISO-2022-CN.
  const bool pair_byte = byte >= 0x21 && byte <= 0x7E;
  if (byte >= 0x80 || (!ext && !pair_byte))
    return Status::INVALID;
  if (inleft < 2) {
    // In ISO-2022-CN, not after a byte past the set's last row of characters.
    const unsigned last = state.read_g0 == Iso2022Set::CNS_PLANE_1  ? 0x7D
                          : state.read_g0 == Iso2022Set::ISO_IR_165 ? 0x7E
                                                                    : 0x77;
    return ext || byte <= last ? Status::INCOMPLETE : Status::INVALID;
  }
  if (!pair_byte)
    return Status::INVALID;
  const char32_t cp = iso2022_cn_character(state.read_g0, byte, in[1]);
  if (cp == 0)
    return Status::INVALID;
  used = 2;
  out = cp;
  return Status::OK;
}

LIBC_INLINE Status write_iso2022_cn(Iso2022State &state, bool ext, char32_t cp,
                                    unsigned char *out, size_t outleft,
                                    size_t &made) {
  made = 0;
  unsigned char bytes[8];
  size_t length = 0;
  if (cp < 0x80) {
    if (state.write_shifted)
      bytes[length++] = SI;
    bytes[length++] = static_cast<unsigned char>(cp);
    if (outleft < length)
      return Status::FULL;
    for (size_t i = 0; i < length; ++i)
      out[i] = bytes[i];
    made = length;
    state.write_shifted = false;
    state.write_last_shifted = false;
    // Each line designates its sets again.
    if (cp == '\n') {
      state.write_g0 = Iso2022Set::ASCII;
      state.write_g2 = Iso2022Set::ASCII;
      state.write_g3 = 0;
    }
    return Status::OK;
  }
  // As in glibc, the set designated for SO is kept while the last character
  // came from it. Otherwise GB 2312 is tried first, or plane 1 if it or
  // ISO-IR-165 was designated, then plane 2, then the sets for SO in order,
  // then planes 3 to 7.
  const Iso2022Set designated = state.write_g0;
  Iso2022Set set = designated;
  uint16_t code = 0;
  unsigned plane = 0;
  if (!state.write_last_shifted)
    set = designated == Iso2022Set::CNS_PLANE_1 ||
                  designated == Iso2022Set::ISO_IR_165
              ? Iso2022Set::CNS_PLANE_1
              : Iso2022Set::GB2312;
  code = iso2022_cn_code(set, cp);
  if (code == 0) {
    code = find_code(CNS_11643[1], cp);
    plane = 2;
  }
  static constexpr Iso2022Set ORDER[] = {
      Iso2022Set::GB2312, Iso2022Set::ISO_IR_165, Iso2022Set::CNS_PLANE_1};
  for (size_t i = 0; i < 3 && code == 0; ++i) {
    if (ORDER[i] == Iso2022Set::ISO_IR_165 && !ext)
      continue;
    set = ORDER[i];
    plane = 0;
    code = iso2022_cn_code(set, cp);
  }
  for (unsigned p = 3; ext && p <= 7 && code == 0; ++p) {
    code = find_code(*cns_plane(p), cp);
    plane = p;
  }
  if (code == 0)
    return Status::INVALID;
  if (plane == 0) {
    if (state.write_g0 != set) {
      bytes[length++] = ESC;
      bytes[length++] = '$';
      bytes[length++] = ')';
      bytes[length++] = set == Iso2022Set::GB2312        ? 'A'
                        : set == Iso2022Set::CNS_PLANE_1 ? 'G'
                                                         : 'E';
    }
    if (!state.write_shifted)
      bytes[length++] = SO;
  } else {
    const bool announced = plane == 2
                               ? state.write_g2 == Iso2022Set::CNS_PLANE_2
                               : state.write_g3 == plane;
    if (!announced) {
      bytes[length++] = ESC;
      bytes[length++] = '$';
      bytes[length++] = plane == 2 ? '*' : '+';
      bytes[length++] = static_cast<unsigned char>('H' + plane - 2);
    }
    bytes[length++] = ESC;
    bytes[length++] = plane == 2 ? 'N' : 'O';
  }
  bytes[length++] = static_cast<unsigned char>(code >> 8);
  bytes[length++] = static_cast<unsigned char>(code & 0xFF);
  if (outleft < length)
    return Status::FULL;
  for (size_t i = 0; i < length; ++i)
    out[i] = bytes[i];
  made = length;
  state.write_last_shifted = plane == 0;
  if (plane == 0) {
    state.write_g0 = set;
    state.write_shifted = true;
  } else if (plane == 2) {
    state.write_g2 = Iso2022Set::CNS_PLANE_2;
  } else {
    state.write_g3 = static_cast<uint8_t>(plane);
  }
  return Status::OK;
}

LIBC_INLINE Status unshift_iso2022_cn(Iso2022State &state, unsigned char *out,
                                      size_t outleft, size_t &made) {
  if (!state.write_shifted)
    return Status::OK;
  if (outleft < 1)
    return Status::FULL;
  out[0] = SI;
  made = 1;
  return Status::OK;
}

} // namespace iconv_internal
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_ICONV_ISO2022_H
