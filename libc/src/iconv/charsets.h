//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// The character sets iconv converts between. Most single byte sets agree with
/// ASCII below 0x80, so only their high half is tabulated, in
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

// The byte of a single byte set which stands for several characters, which
// the set's sequences give.
constexpr uint16_t MULTIPLE = 0xFFFF;

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
  SINGLE_BYTE,      // A table of the bytes from 0x80 up.
  SINGLE_BYTE_FULL, // A table of all 256 bytes, for a set which is not ASCII
                    // below 0x80.
};

struct Charset {
  const char *name;
  Encoding encoding;
  const uint16_t *table; // Only for the single byte sets.
  // Only for a set which joins a letter and the marks after it.
  const Combining *combining = nullptr;
  // Only for a set with codes which stand for several characters.
  const Sequences *sequences = nullptr;
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
    {"CP1250", Encoding::SINGLE_BYTE, CP1250_HIGH},
    {"WINDOWS1250", Encoding::SINGLE_BYTE, CP1250_HIGH},
    {"MSEE", Encoding::SINGLE_BYTE, CP1250_HIGH},
    {"CP1253", Encoding::SINGLE_BYTE, CP1253_HIGH},
    {"WINDOWS1253", Encoding::SINGLE_BYTE, CP1253_HIGH},
    {"MSGREEK", Encoding::SINGLE_BYTE, CP1253_HIGH},
    {"CP1254", Encoding::SINGLE_BYTE, CP1254_HIGH},
    {"WINDOWS1254", Encoding::SINGLE_BYTE, CP1254_HIGH},
    {"MSTURK", Encoding::SINGLE_BYTE, CP1254_HIGH},
    {"CP1256", Encoding::SINGLE_BYTE, CP1256_HIGH},
    {"WINDOWS1256", Encoding::SINGLE_BYTE, CP1256_HIGH},
    {"MSARAB", Encoding::SINGLE_BYTE, CP1256_HIGH},
    {"CP1257", Encoding::SINGLE_BYTE, CP1257_HIGH},
    {"WINDOWS1257", Encoding::SINGLE_BYTE, CP1257_HIGH},
    {"WINBALTRIM", Encoding::SINGLE_BYTE, CP1257_HIGH},
    {"CP874", Encoding::SINGLE_BYTE, CP874_HIGH},
    {"WINDOWS874", Encoding::SINGLE_BYTE, CP874_HIGH},
    {"CP862", Encoding::SINGLE_BYTE, CP862_HIGH},
    {"IBM862", Encoding::SINGLE_BYTE, CP862_HIGH},
    {"862", Encoding::SINGLE_BYTE, CP862_HIGH},
    {"CSPC862LATINHEBREW", Encoding::SINGLE_BYTE, CP862_HIGH},
    {"CP866", Encoding::SINGLE_BYTE, CP866_HIGH},
    {"IBM866", Encoding::SINGLE_BYTE, CP866_HIGH},
    {"866", Encoding::SINGLE_BYTE, CP866_HIGH},
    {"CSIBM866", Encoding::SINGLE_BYTE, CP866_HIGH},
    {"KOI8U", Encoding::SINGLE_BYTE, KOI8_U_HIGH},
    {"RK1048", Encoding::SINGLE_BYTE, RK1048_HIGH},
    {"STRK10482002", Encoding::SINGLE_BYTE, RK1048_HIGH},
    {"KZ1048", Encoding::SINGLE_BYTE, RK1048_HIGH},
    {"CSKZ1048", Encoding::SINGLE_BYTE, RK1048_HIGH},
    {"KOI8RU", Encoding::SINGLE_BYTE, KOI8_RU_HIGH},
    {"KOI8T", Encoding::SINGLE_BYTE, KOI8_T_HIGH},
    {"PT154", Encoding::SINGLE_BYTE, PT154_HIGH},
    {"PTCP154", Encoding::SINGLE_BYTE, PT154_HIGH},
    {"CP154", Encoding::SINGLE_BYTE, PT154_HIGH},
    {"CYRILLICASIAN", Encoding::SINGLE_BYTE, PT154_HIGH},
    {"CSPTCP154", Encoding::SINGLE_BYTE, PT154_HIGH},
    {"GEORGIANACADEMY", Encoding::SINGLE_BYTE, GEORGIAN_ACADEMY_HIGH},
    {"GEORGIANPS", Encoding::SINGLE_BYTE, GEORGIAN_PS_HIGH},
    {"HPROMAN8", Encoding::SINGLE_BYTE, HP_ROMAN8_HIGH},
    {"ROMAN8", Encoding::SINGLE_BYTE, HP_ROMAN8_HIGH},
    {"R8", Encoding::SINGLE_BYTE, HP_ROMAN8_HIGH},
    {"CSHPROMAN8", Encoding::SINGLE_BYTE, HP_ROMAN8_HIGH},
    {"CP1131", Encoding::SINGLE_BYTE, CP1131_HIGH},
    {"MULELAO1", Encoding::SINGLE_BYTE, MULELAO_1_HIGH},
    {"CP1133", Encoding::SINGLE_BYTE, CP1133_HIGH},
    {"IBMCP1133", Encoding::SINGLE_BYTE, CP1133_HIGH},
    {"TIS620", Encoding::SINGLE_BYTE, TIS_620_HIGH},
    {"TIS6200", Encoding::SINGLE_BYTE, TIS_620_HIGH},
    {"TIS620.25291", Encoding::SINGLE_BYTE, TIS_620_HIGH},
    {"TIS620.25330", Encoding::SINGLE_BYTE, TIS_620_HIGH},
    {"TIS620.25331", Encoding::SINGLE_BYTE, TIS_620_HIGH},
    {"ISOIR166", Encoding::SINGLE_BYTE, TIS_620_HIGH},
    {"NEXTSTEP", Encoding::SINGLE_BYTE, NEXTSTEP_HIGH},
    {"ARMSCII8", Encoding::SINGLE_BYTE, ARMSCII_8_HIGH},
    {"JISC62201969RO", Encoding::SINGLE_BYTE_FULL, JIS_C6220_1969_RO_FULL},
    {"ISO646JP", Encoding::SINGLE_BYTE_FULL, JIS_C6220_1969_RO_FULL},
    {"ISOIR14", Encoding::SINGLE_BYTE_FULL, JIS_C6220_1969_RO_FULL},
    {"JP", Encoding::SINGLE_BYTE_FULL, JIS_C6220_1969_RO_FULL},
    {"CSISO14JISC6220RO", Encoding::SINGLE_BYTE_FULL, JIS_C6220_1969_RO_FULL},
    {"GB198880", Encoding::SINGLE_BYTE_FULL, GB_1988_80_FULL},
    {"ISO646CN", Encoding::SINGLE_BYTE_FULL, GB_1988_80_FULL},
    {"ISOIR57", Encoding::SINGLE_BYTE_FULL, GB_1988_80_FULL},
    {"CN", Encoding::SINGLE_BYTE_FULL, GB_1988_80_FULL},
    {"CSISO57GB1988", Encoding::SINGLE_BYTE_FULL, GB_1988_80_FULL},
    {"VISCII", Encoding::SINGLE_BYTE_FULL, VISCII_FULL},
    {"VISCII1.11", Encoding::SINGLE_BYTE_FULL, VISCII_FULL},
    {"CSVISCII", Encoding::SINGLE_BYTE_FULL, VISCII_FULL},
    {"JISX0201", Encoding::SINGLE_BYTE_FULL, JIS_X0201_FULL},
    {"JISX02011976", Encoding::SINGLE_BYTE_FULL, JIS_X0201_FULL},
    {"X0201", Encoding::SINGLE_BYTE_FULL, JIS_X0201_FULL},
    {"CSHALFWIDTHKATAKANA", Encoding::SINGLE_BYTE_FULL, JIS_X0201_FULL},
    {"MACROMAN", Encoding::SINGLE_BYTE, MAC_ROMAN_HIGH},
    {"MACINTOSH", Encoding::SINGLE_BYTE, MAC_ROMAN_HIGH},
    {"MAC", Encoding::SINGLE_BYTE, MAC_ROMAN_HIGH},
    {"CSMACINTOSH", Encoding::SINGLE_BYTE, MAC_ROMAN_HIGH},
    {"MACCENTRALEUROPE", Encoding::SINGLE_BYTE, MAC_CENTRAL_EUROPE_HIGH},
    {"MACICELAND", Encoding::SINGLE_BYTE, MAC_ICELAND_HIGH},
    {"MACCROATIAN", Encoding::SINGLE_BYTE, MAC_CROATIAN_HIGH},
    {"MACROMANIA", Encoding::SINGLE_BYTE, MAC_ROMANIA_HIGH},
    {"MACCYRILLIC", Encoding::SINGLE_BYTE, MAC_CYRILLIC_HIGH},
    {"MACUKRAINE", Encoding::SINGLE_BYTE, MAC_UKRAINE_HIGH},
    {"MACUK", Encoding::SINGLE_BYTE, MAC_UKRAINE_HIGH},
    {"MACGREEK", Encoding::SINGLE_BYTE, MAC_GREEK_HIGH},
    {"MACTURKISH", Encoding::SINGLE_BYTE, MAC_TURKISH_HIGH},
    {"MACARABIC", Encoding::SINGLE_BYTE, MAC_ARABIC_HIGH},
    {"MACHEBREW", Encoding::SINGLE_BYTE, MAC_HEBREW_HIGH, nullptr,
     &MAC_HEBREW_SEQUENCES},
    {"MACTHAI", Encoding::SINGLE_BYTE, MAC_THAI_HIGH, nullptr,
     &MAC_THAI_SEQUENCES},
    {"CP1258", Encoding::SINGLE_BYTE, CP1258_HIGH, &CP1258_COMBINING},
    {"WINDOWS1258", Encoding::SINGLE_BYTE, CP1258_HIGH, &CP1258_COMBINING},
    {"CP1255", Encoding::SINGLE_BYTE, CP1255_HIGH, &CP1255_COMBINING},
    {"WINDOWS1255", Encoding::SINGLE_BYTE, CP1255_HIGH, &CP1255_COMBINING},
    {"MSHEBR", Encoding::SINGLE_BYTE, CP1255_HIGH, &CP1255_COMBINING},
    {"TCVN", Encoding::SINGLE_BYTE_FULL, TCVN_FULL, &TCVN_COMBINING},
    {"TCVN5712", Encoding::SINGLE_BYTE_FULL, TCVN_FULL, &TCVN_COMBINING},
    {"TCVN57121", Encoding::SINGLE_BYTE_FULL, TCVN_FULL, &TCVN_COMBINING},
    {"TCVN57121:1993", Encoding::SINGLE_BYTE_FULL, TCVN_FULL, &TCVN_COMBINING},
};

constexpr size_t CHARSET_COUNT = sizeof(CHARSETS) / sizeof(CHARSETS[0]);

} // namespace iconv_internal
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_ICONV_CHARSETS_H
