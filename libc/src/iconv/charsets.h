//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// The character sets iconv converts between. A single byte set agrees with
/// ASCII below 0x80, so only its high half is tabulated, in
/// single_byte_tables.h, which libc/utils/iconv_utils/gen.py generates from the
/// mapping files the vendors publish. 0xFFFD marks a byte the set does not
/// assign.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_ICONV_CHARSETS_H
#define LLVM_LIBC_SRC_ICONV_CHARSETS_H

#include "hdr/stdint_proxy.h"
#include "hdr/types/size_t.h"
#include "src/__support/endian_internal.h"
#include "src/__support/macros/attributes.h"
#include "src/__support/macros/config.h"
#include "src/iconv/single_byte_tables.h"

namespace LIBC_NAMESPACE_DECL {
namespace iconv_internal {

// The byte a single byte set has no character for.
constexpr uint16_t UNASSIGNED = 0xFFFD;

// How the bytes of a conversion are laid out. The multibyte forms are
// handled in code; the single byte ones are a table lookup.
enum class Encoding {
  UTF8,
  UTF16LE,
  UTF16BE,
  UTF32LE,
  UTF32BE,
  // UCS-2 is UTF-16 without surrogates, so it cannot hold a character past
  // U+FFFF.
  UCS2LE,
  UCS2BE,
  // UTF-16, UTF-32 and UCS-2 whose byte order a mark at the start of the input
  // may give. Output is in the host's order and begins with the mark.
  UTF16,
  UTF32,
  UCS2_BOM,
  // ASCII with every other character written as a \u escape, the way C99 and
  // Java source name them.
  C99,
  JAVA,
  // UTF-16 written in ASCII, with base64 runs for what cannot be written as
  // it is.
  UTF7,
  ASCII,
  SINGLE_BYTE, // One of the tables above.
};

struct Charset {
  const char *name;
  Encoding encoding;
  const uint16_t *table; // Only for SINGLE_BYTE.
};

// The names iconv_open accepts: every name GNU libiconv gives these sets, and
// glibc's "UNICODE" and "WCHAR_T". A name is matched without regard to case,
// and to the punctuation which separates the parts, so "ISO-8859-1",
// "iso88591" and "ISO_8859-1" are all the same set. The internal and swapped
// forms are in the host's byte order and the other one.
constexpr Charset CHARSETS[] = {
    {"UTF8", Encoding::UTF8, nullptr},
    {"UCS2", Endian::IS_LITTLE ? Encoding::UCS2LE : Encoding::UCS2BE, nullptr},
    {"ISO10646UCS2", Endian::IS_LITTLE ? Encoding::UCS2LE : Encoding::UCS2BE,
     nullptr},
    // glibc reads "csUnicode" as its "UNICODE", which libiconv does not.
    {"CSUNICODE", Encoding::UCS2_BOM, nullptr},
    {"UCS2BE", Encoding::UCS2BE, nullptr},
    {"UNICODEBIG", Encoding::UCS2BE, nullptr},
    {"UNICODE11", Encoding::UCS2BE, nullptr},
    {"CSUNICODE11", Encoding::UCS2BE, nullptr},
    {"UCS2LE", Encoding::UCS2LE, nullptr},
    {"UNICODELITTLE", Encoding::UCS2LE, nullptr},
    {"UCS4", Encoding::UTF32BE, nullptr},
    {"ISO10646UCS4", Encoding::UTF32BE, nullptr},
    {"CSUCS4", Encoding::UTF32BE, nullptr},
    {"UCS4BE", Encoding::UTF32BE, nullptr},
    {"UCS4LE", Encoding::UTF32LE, nullptr},
    {"UTF16", Encoding::UTF16, nullptr},
    {"UTF16BE", Encoding::UTF16BE, nullptr},
    {"UTF16LE", Encoding::UTF16LE, nullptr},
    {"UTF32", Encoding::UTF32, nullptr},
    {"UTF32BE", Encoding::UTF32BE, nullptr},
    {"UTF32LE", Encoding::UTF32LE, nullptr},
    {"UCS2INTERNAL", Endian::IS_LITTLE ? Encoding::UCS2LE : Encoding::UCS2BE,
     nullptr},
    {"UCS2SWAPPED", Endian::IS_LITTLE ? Encoding::UCS2BE : Encoding::UCS2LE,
     nullptr},
    {"UCS4INTERNAL", Endian::IS_LITTLE ? Encoding::UTF32LE : Encoding::UTF32BE,
     nullptr},
    {"UCS4SWAPPED", Endian::IS_LITTLE ? Encoding::UTF32BE : Encoding::UTF32LE,
     nullptr},
    {"USASCII", Encoding::ASCII, nullptr},
    {"ASCII", Encoding::ASCII, nullptr},
    {"ISO646US", Encoding::ASCII, nullptr},
    {"ISO646.IRV:1991", Encoding::ASCII, nullptr},
    {"ISOIR6", Encoding::ASCII, nullptr},
    {"ANSIX3.41968", Encoding::ASCII, nullptr},
    {"ANSIX3.41986", Encoding::ASCII, nullptr},
    {"CP367", Encoding::ASCII, nullptr},
    {"IBM367", Encoding::ASCII, nullptr},
    {"US", Encoding::ASCII, nullptr},
    {"CSASCII", Encoding::ASCII, nullptr},
    {"ISO88591", Encoding::SINGLE_BYTE, ISO8859_1_HIGH},
    {"ISO88591:1987", Encoding::SINGLE_BYTE, ISO8859_1_HIGH},
    {"ISOIR100", Encoding::SINGLE_BYTE, ISO8859_1_HIGH},
    {"CP819", Encoding::SINGLE_BYTE, ISO8859_1_HIGH},
    {"IBM819", Encoding::SINGLE_BYTE, ISO8859_1_HIGH},
    {"LATIN1", Encoding::SINGLE_BYTE, ISO8859_1_HIGH},
    {"L1", Encoding::SINGLE_BYTE, ISO8859_1_HIGH},
    {"CSISOLATIN1", Encoding::SINGLE_BYTE, ISO8859_1_HIGH},
    {"ISO88592", Encoding::SINGLE_BYTE, ISO8859_2_HIGH},
    {"ISO88592:1987", Encoding::SINGLE_BYTE, ISO8859_2_HIGH},
    {"ISOIR101", Encoding::SINGLE_BYTE, ISO8859_2_HIGH},
    {"LATIN2", Encoding::SINGLE_BYTE, ISO8859_2_HIGH},
    {"L2", Encoding::SINGLE_BYTE, ISO8859_2_HIGH},
    {"CSISOLATIN2", Encoding::SINGLE_BYTE, ISO8859_2_HIGH},
    {"ISO88593", Encoding::SINGLE_BYTE, ISO8859_3_HIGH},
    {"ISO88593:1988", Encoding::SINGLE_BYTE, ISO8859_3_HIGH},
    {"ISOIR109", Encoding::SINGLE_BYTE, ISO8859_3_HIGH},
    {"LATIN3", Encoding::SINGLE_BYTE, ISO8859_3_HIGH},
    {"L3", Encoding::SINGLE_BYTE, ISO8859_3_HIGH},
    {"CSISOLATIN3", Encoding::SINGLE_BYTE, ISO8859_3_HIGH},
    {"ISO88594", Encoding::SINGLE_BYTE, ISO8859_4_HIGH},
    {"ISO88594:1988", Encoding::SINGLE_BYTE, ISO8859_4_HIGH},
    {"ISOIR110", Encoding::SINGLE_BYTE, ISO8859_4_HIGH},
    {"LATIN4", Encoding::SINGLE_BYTE, ISO8859_4_HIGH},
    {"L4", Encoding::SINGLE_BYTE, ISO8859_4_HIGH},
    {"CSISOLATIN4", Encoding::SINGLE_BYTE, ISO8859_4_HIGH},
    {"ISO88595", Encoding::SINGLE_BYTE, ISO8859_5_HIGH},
    {"ISO88595:1988", Encoding::SINGLE_BYTE, ISO8859_5_HIGH},
    {"ISOIR144", Encoding::SINGLE_BYTE, ISO8859_5_HIGH},
    {"CYRILLIC", Encoding::SINGLE_BYTE, ISO8859_5_HIGH},
    {"CSISOLATINCYRILLIC", Encoding::SINGLE_BYTE, ISO8859_5_HIGH},
    {"ISO88596", Encoding::SINGLE_BYTE, ISO8859_6_HIGH},
    {"ISO88596:1987", Encoding::SINGLE_BYTE, ISO8859_6_HIGH},
    {"ISOIR127", Encoding::SINGLE_BYTE, ISO8859_6_HIGH},
    {"ECMA114", Encoding::SINGLE_BYTE, ISO8859_6_HIGH},
    {"ASMO708", Encoding::SINGLE_BYTE, ISO8859_6_HIGH},
    {"ARABIC", Encoding::SINGLE_BYTE, ISO8859_6_HIGH},
    {"CSISOLATINARABIC", Encoding::SINGLE_BYTE, ISO8859_6_HIGH},
    {"ISO88597", Encoding::SINGLE_BYTE, ISO8859_7_HIGH},
    {"ISO88597:1987", Encoding::SINGLE_BYTE, ISO8859_7_HIGH},
    {"ISO88597:2003", Encoding::SINGLE_BYTE, ISO8859_7_HIGH},
    {"ISOIR126", Encoding::SINGLE_BYTE, ISO8859_7_HIGH},
    {"ECMA118", Encoding::SINGLE_BYTE, ISO8859_7_HIGH},
    {"ELOT928", Encoding::SINGLE_BYTE, ISO8859_7_HIGH},
    {"GREEK8", Encoding::SINGLE_BYTE, ISO8859_7_HIGH},
    {"GREEK", Encoding::SINGLE_BYTE, ISO8859_7_HIGH},
    {"CSISOLATINGREEK", Encoding::SINGLE_BYTE, ISO8859_7_HIGH},
    {"ISO88598", Encoding::SINGLE_BYTE, ISO8859_8_HIGH},
    {"ISO88598:1988", Encoding::SINGLE_BYTE, ISO8859_8_HIGH},
    {"ISOIR138", Encoding::SINGLE_BYTE, ISO8859_8_HIGH},
    {"HEBREW", Encoding::SINGLE_BYTE, ISO8859_8_HIGH},
    {"CSISOLATINHEBREW", Encoding::SINGLE_BYTE, ISO8859_8_HIGH},
    {"ISO88599", Encoding::SINGLE_BYTE, ISO8859_9_HIGH},
    {"ISO88599:1989", Encoding::SINGLE_BYTE, ISO8859_9_HIGH},
    {"ISOIR148", Encoding::SINGLE_BYTE, ISO8859_9_HIGH},
    {"LATIN5", Encoding::SINGLE_BYTE, ISO8859_9_HIGH},
    {"L5", Encoding::SINGLE_BYTE, ISO8859_9_HIGH},
    {"CSISOLATIN5", Encoding::SINGLE_BYTE, ISO8859_9_HIGH},
    {"ISO885910", Encoding::SINGLE_BYTE, ISO8859_10_HIGH},
    {"ISO885910:1992", Encoding::SINGLE_BYTE, ISO8859_10_HIGH},
    {"ISOIR157", Encoding::SINGLE_BYTE, ISO8859_10_HIGH},
    {"LATIN6", Encoding::SINGLE_BYTE, ISO8859_10_HIGH},
    {"L6", Encoding::SINGLE_BYTE, ISO8859_10_HIGH},
    {"CSISOLATIN6", Encoding::SINGLE_BYTE, ISO8859_10_HIGH},
    {"ISO885911", Encoding::SINGLE_BYTE, ISO8859_11_HIGH},
    {"ISO885913", Encoding::SINGLE_BYTE, ISO8859_13_HIGH},
    {"ISOIR179", Encoding::SINGLE_BYTE, ISO8859_13_HIGH},
    {"LATIN7", Encoding::SINGLE_BYTE, ISO8859_13_HIGH},
    {"L7", Encoding::SINGLE_BYTE, ISO8859_13_HIGH},
    {"ISO885914", Encoding::SINGLE_BYTE, ISO8859_14_HIGH},
    {"ISO885914:1998", Encoding::SINGLE_BYTE, ISO8859_14_HIGH},
    {"ISOIR199", Encoding::SINGLE_BYTE, ISO8859_14_HIGH},
    {"LATIN8", Encoding::SINGLE_BYTE, ISO8859_14_HIGH},
    {"L8", Encoding::SINGLE_BYTE, ISO8859_14_HIGH},
    {"ISOCELTIC", Encoding::SINGLE_BYTE, ISO8859_14_HIGH},
    {"ISO885915", Encoding::SINGLE_BYTE, ISO8859_15_HIGH},
    {"ISO885915:1998", Encoding::SINGLE_BYTE, ISO8859_15_HIGH},
    {"ISOIR203", Encoding::SINGLE_BYTE, ISO8859_15_HIGH},
    {"LATIN9", Encoding::SINGLE_BYTE, ISO8859_15_HIGH},
    {"ISO885916", Encoding::SINGLE_BYTE, ISO8859_16_HIGH},
    {"ISO885916:2001", Encoding::SINGLE_BYTE, ISO8859_16_HIGH},
    {"ISOIR226", Encoding::SINGLE_BYTE, ISO8859_16_HIGH},
    {"LATIN10", Encoding::SINGLE_BYTE, ISO8859_16_HIGH},
    {"L10", Encoding::SINGLE_BYTE, ISO8859_16_HIGH},
    {"KOI8R", Encoding::SINGLE_BYTE, KOI8_R_HIGH},
    {"CSKOI8R", Encoding::SINGLE_BYTE, KOI8_R_HIGH},
    {"CP1251", Encoding::SINGLE_BYTE, CP1251_HIGH},
    {"WINDOWS1251", Encoding::SINGLE_BYTE, CP1251_HIGH},
    {"MSCYRL", Encoding::SINGLE_BYTE, CP1251_HIGH},
    {"CP1252", Encoding::SINGLE_BYTE, CP1252_HIGH},
    {"WINDOWS1252", Encoding::SINGLE_BYTE, CP1252_HIGH},
    {"MSANSI", Encoding::SINGLE_BYTE, CP1252_HIGH},
    {"CP850", Encoding::SINGLE_BYTE, CP850_HIGH},
    {"IBM850", Encoding::SINGLE_BYTE, CP850_HIGH},
    {"850", Encoding::SINGLE_BYTE, CP850_HIGH},
    {"CSPC850MULTILINGUAL", Encoding::SINGLE_BYTE, CP850_HIGH},
    {"UNICODE", Encoding::UCS2_BOM, nullptr},
    {"UTF7", Encoding::UTF7, nullptr},
    {"UNICODE11UTF7", Encoding::UTF7, nullptr},
    {"CSUNICODE11UTF7", Encoding::UTF7, nullptr},
    {"C99", Encoding::C99, nullptr},
    {"JAVA", Encoding::JAVA, nullptr},
    {"WCHART", Endian::IS_LITTLE ? Encoding::UTF32LE : Encoding::UTF32BE,
     nullptr},
    {"CP437", Encoding::SINGLE_BYTE, CP437_HIGH},
};

constexpr size_t CHARSET_COUNT = sizeof(CHARSETS) / sizeof(CHARSETS[0]);

} // namespace iconv_internal
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_ICONV_CHARSETS_H
