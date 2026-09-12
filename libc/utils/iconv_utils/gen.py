#!/usr/bin/env python3
#
# ===- Generate the iconv character set tables ---------------*- python -*--==#
#
# Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# ==------------------------------------------------------------------------==#

import re
from sys import argv, exit

UNICODE = "https://www.unicode.org/Public/MAPPINGS"
UCD = "https://www.unicode.org/Public/UCD/latest/ucd"
EASTASIA = f"{UNICODE}/OBSOLETE/EASTASIA"
CITRUS = "https://github.com/freebsd/freebsd-src/tree/main/share/i18n/csmapper"
ICU = "https://github.com/unicode-org/icu-data/tree/main/charset/data/ucm"

# The single byte sets: the name of the table, the mapping file it is read
# from, and where that file is published. Each of these sets agrees with ASCII
# below 0x80, so only the high half is written out.
SINGLE_BYTE = [
    ("ISO8859_1", "8859-1.TXT", f"{UNICODE}/ISO8859"),
    ("ISO8859_2", "8859-2.TXT", f"{UNICODE}/ISO8859"),
    ("ISO8859_3", "8859-3.TXT", f"{UNICODE}/ISO8859"),
    ("ISO8859_4", "8859-4.TXT", f"{UNICODE}/ISO8859"),
    ("ISO8859_5", "8859-5.TXT", f"{UNICODE}/ISO8859"),
    ("ISO8859_6", "8859-6.TXT", f"{UNICODE}/ISO8859"),
    ("ISO8859_7", "8859-7.TXT", f"{UNICODE}/ISO8859"),
    ("ISO8859_8", "8859-8.TXT", f"{UNICODE}/ISO8859"),
    ("ISO8859_9", "8859-9.TXT", f"{UNICODE}/ISO8859"),
    ("ISO8859_10", "8859-10.TXT", f"{UNICODE}/ISO8859"),
    ("ISO8859_11", "8859-11.TXT", f"{UNICODE}/ISO8859"),
    ("ISO8859_13", "8859-13.TXT", f"{UNICODE}/ISO8859"),
    ("ISO8859_14", "8859-14.TXT", f"{UNICODE}/ISO8859"),
    ("ISO8859_15", "8859-15.TXT", f"{UNICODE}/ISO8859"),
    ("ISO8859_16", "8859-16.TXT", f"{UNICODE}/ISO8859"),
    ("CP1252", "CP1252.TXT", f"{UNICODE}/VENDORS/MICSFT/WINDOWS"),
    ("CP1251", "CP1251.TXT", f"{UNICODE}/VENDORS/MICSFT/WINDOWS"),
    ("KOI8_R", "KOI8-R.TXT", f"{UNICODE}/VENDORS/MISC"),
    ("CP437", "CP437.TXT", f"{UNICODE}/VENDORS/MICSFT/PC"),
    ("CP850", "CP850.TXT", f"{UNICODE}/VENDORS/MICSFT/PC"),
    ("CP1250", "CP1250.TXT", f"{UNICODE}/VENDORS/MICSFT/WINDOWS"),
    ("CP1253", "CP1253.TXT", f"{UNICODE}/VENDORS/MICSFT/WINDOWS"),
    ("CP1254", "CP1254.TXT", f"{UNICODE}/VENDORS/MICSFT/WINDOWS"),
    ("CP1256", "CP1256.TXT", f"{UNICODE}/VENDORS/MICSFT/WINDOWS"),
    ("CP1257", "CP1257.TXT", f"{UNICODE}/VENDORS/MICSFT/WINDOWS"),
    ("CP874", "CP874.TXT", f"{UNICODE}/VENDORS/MICSFT/WINDOWS"),
    ("CP862", "CP862.TXT", f"{UNICODE}/VENDORS/MICSFT/PC"),
    ("CP866", "CP866.TXT", f"{UNICODE}/VENDORS/MICSFT/PC"),
    ("KOI8_U", "KOI8-U.TXT", f"{UNICODE}/VENDORS/MISC"),
    ("RK1048", "KZ1048.TXT", f"{UNICODE}/VENDORS/MISC"),
    # KOI8-T is the KOI8-R letters from 0xC0 up, with the Tajik set's own
    # characters below them. Citrus also assigns some C1 controls and Latin-1
    # symbols there, which glibc and GNU libiconv leave unassigned.
    (
        "KOI8_T",
        "KOI8-R.TXT",
        f"{UNICODE}/VENDORS/MISC",
        {
            "below": 0x80,
            "overlay": ("KOI8-T%UCS.src", f"{CITRUS}/KOI"),
            "unassigned": [
                0x88,
                0x8F,
                0x98,
                0x9A,
                0x9C,
                0x9D,
                0x9E,
                0x9F,
                0xA0,
                0xA8,
                0xA9,
                0xAA,
                0xAF,
                0xB4,
                0xB8,
                0xBA,
                0xBC,
                0xBD,
                0xBE,
            ],
        },
    ),
    # KOI8-RU as glibc has it: KOI8-U with the Belarusian short u, and the
    # punctuation from 0x93 to 0x9F where KOI8-U has box drawing.
    (
        "KOI8_RU",
        "KOI8-U.TXT",
        f"{UNICODE}/VENDORS/MISC",
        {
            "changes": {
                0x93: 0x201C,
                0x96: 0x201D,
                0x97: 0x2014,
                0x98: 0x2116,
                0x99: 0x2122,
                0x9B: 0x00BB,
                0x9C: 0x00AE,
                0x9D: 0x00AB,
                0x9F: 0x00A4,
                0xAE: 0x045E,
                0xBE: 0x040E,
            },
        },
    ),
    ("PT154", "PTCP154%UCS.src", f"{CITRUS}/KAZAKH"),
    ("GEORGIAN_ACADEMY", "GEORGIAN-ACADEMY%UCS.src", f"{CITRUS}/GEORGIAN"),
    ("GEORGIAN_PS", "GEORGIAN-PS%UCS.src", f"{CITRUS}/GEORGIAN"),
    ("HP_ROMAN8", "HP-ROMAN8%UCS.src", f"{CITRUS}/MISC"),
    ("CP1131", "CP1131%UCS.src", f"{CITRUS}/CP"),
    # MuleLao-1 has no vendor. GNU libiconv, the one implementation, assigns
    # neither 0xDF nor 0xFB.
    ("MULELAO_1", "MULELAO-1%UCS.src", f"{CITRUS}/MISC", {"unassigned": [0xDF, 0xFB]}),
    # IBM's own table, which glibc follows.
    ("CP1133", "ibm-1133_P100-1997.ucm", ICU),
    # TIS-620 is ISO-8859-11 without its C1 controls and no-break space.
    (
        "TIS_620",
        "8859-11.TXT",
        f"{UNICODE}/ISO8859",
        {"unassigned": list(range(0x80, 0xA1))},
    ),
]

SINGLE_BYTE += [
    ("NEXTSTEP", "NEXTSTEP.TXT", f"{UNICODE}/VENDORS/NEXT"),
    # Citrus gives 0xA1 a private use character it is unsure of, and 0xFF an
    # apostrophe; glibc and GNU libiconv leave both unassigned.
    (
        "ARMSCII_8",
        "ARMSCII-8%UCS.src",
        f"{CITRUS}/AST",
        {"unassigned": [0xA1, 0xFF]},
    ),
]

APPLE = f"{UNICODE}/VENDORS/APPLE"

# Apple's Mac OS sets, as Apple's current tables give them. They leave out the
# control characters, which these sets share with ASCII.
SINGLE_BYTE += [
    ("MAC_ROMAN", "APPLE-ROMAN.TXT", APPLE, {"controls": True}),
    ("MAC_CENTRAL_EUROPE", "APPLE-CENTEURO.TXT", APPLE, {"controls": True}),
    ("MAC_ICELAND", "APPLE-ICELAND.TXT", APPLE, {"controls": True}),
    ("MAC_CROATIAN", "APPLE-CROATIAN.TXT", APPLE, {"controls": True}),
    ("MAC_ROMANIA", "APPLE-ROMANIAN.TXT", APPLE, {"controls": True}),
    ("MAC_CYRILLIC", "APPLE-CYRILLIC.TXT", APPLE, {"controls": True}),
    # Apple publishes no table of its own for Mac OS Ukrainian. The notes in its
    # Cyrillic table give it as that table with the currency sign at 0xFF,
    # where Mac OS 9 put the euro sign.
    (
        "MAC_UKRAINE",
        "APPLE-CYRILLIC.TXT",
        APPLE,
        {"controls": True, "changes": {0xFF: 0x00A4}},
    ),
    ("MAC_GREEK", "APPLE-GREEK.TXT", APPLE, {"controls": True}),
    ("MAC_TURKISH", "APPLE-TURKISH.TXT", APPLE, {"controls": True}),
    ("MAC_ARABIC", "APPLE-ARABIC.TXT", APPLE, {"controls": True}),
    # Apple gives some bytes of these as several characters: a ligature as its
    # letters and points, or a character and one of Apple's transcoding hints,
    # which mark a group, a variant or where to draw it. Such a byte reads as
    # all of them.
    ("MAC_HEBREW", "APPLE-HEBREW.TXT", APPLE, {"controls": True}),
    ("MAC_THAI", "APPLE-THAI.TXT", APPLE, {"controls": True}),
]

# The sets which write some characters as a letter followed by combining marks.
# See COMBINING.
SINGLE_BYTE += [
    ("CP1258", "CP1258.TXT", f"{UNICODE}/VENDORS/MICSFT/WINDOWS"),
    # Windows has since assigned 0xCA, as Microsoft's best fit table for the
    # set gives.
    (
        "CP1255",
        "CP1255.TXT",
        f"{UNICODE}/VENDORS/MICSFT/WINDOWS",
        {"changes": {0xCA: 0x05BA}},
    ),
]

# The sets which do not agree with ASCII below 0x80, so are written out for
# all 256 bytes. The same rules apply as for SINGLE_BYTE.
FULL = [
    ("JIS_C6220_1969_RO", "ISO646-JP%UCS.646", f"{CITRUS}/ISO646"),
    ("GB_1988_80", "ISO646-CN%UCS.646", f"{CITRUS}/ISO646"),
    ("VISCII", "VISCII%UCS.src", f"{CITRUS}/TCVN"),
    # The file leaves out the control characters, which JIS X 0201 shares
    # with ASCII.
    (
        "JIS_X0201",
        "JIS0201.TXT",
        f"{UNICODE}/OBSOLETE/EASTASIA/JIS",
        {"controls": True},
    ),
    # A table of two bytes, a row for each byte and a column for the one after
    # it. Row 0 gives each byte alone, except for a letter which a mark may
    # follow, which is given in its own row.
    ("TCVN", "TCVN5712-1%UCS.src", f"{CITRUS}/TCVN", {"rows": True}),
]

# The sets which join a letter and the combining mark after it into one
# character, and write a character they have no byte for as a letter and marks.
# What they join is what UnicodeData.txt, from the Unicode Character Database
# at UCD, gives as canonically equivalent: the letter may already be joined,
# and a character which decomposes to a single other one is only a second name
# for it. That takes in the Hebrew presentation forms, which Unicode leaves out
# of NFC but glibc joins. It leaves out O, U and O with a diaeresis followed by
# a tilde, which glibc and Citrus's TCVN table join to the letters with a tilde
# and then another mark: marks of the same class in another order are
# different text.
COMBINING = ["CP1258", "CP1255", "TCVN"]

# The Japanese sets. The Unicode Consortium's JIS X 0208 and JIS X 0212 tables
# map 0x2140 and 0x2237 to ASCII's reverse solidus and tilde. glibc and GNU
# libiconv map them to the fullwidth forms, so that EUC-JP, which has ASCII as
# well, can write both.
JIS_X0208 = ("JIS0208.TXT", f"{EASTASIA}/JIS", {0x2140: 0xFF3C})
JIS_X0212 = ("JIS0212.TXT", f"{EASTASIA}/JIS", {0x2237: 0xFF5E})
# CP932 reads as Microsoft's table has it. Where several codes read as the same
# character, it is written as the one Microsoft's best fit table gives, and the
# others are only read.
CP932 = ("CP932.TXT", "bestfit932.txt", f"{UNICODE}/VENDORS/MICSFT")

# The Simplified Chinese sets share one table of two byte codes: GB18030's, as
# ICU's table for GB18030-2022 has it. GBK and CP936, which glibc treats as the
# same set, are Microsoft's CP936, whose two byte codes are all GB18030's too.
# EUC-CN is GB 2312 as Citrus has it.
ICU_MAPPINGS = "https://github.com/unicode-org/icu/tree/main/icu4c/source/data/mappings"
GB18030 = ("gb18030-2022.ucm", ICU_MAPPINGS)
CP936 = ("CP936.TXT", f"{UNICODE}/VENDORS/MICSFT/WINDOWS")
GB2312 = ("GB2312%UCS.src", f"{CITRUS}/GB")
# ISO-IR-165 is GB 2312 with the additions of GB 6345.1 and GB 8565.2, as
# Citrus has them, and GB 1988 in row 0x2A, as glibc and GNU libiconv have it
# where Citrus leaves the row out. 0x283C is U+1E3F, where Citrus has a private
# use character. glibc and GNU libiconv read a few codes otherwise: glibc five
# of GB 2312's, 0x283F, 0x2840 and 0x2A67, GNU libiconv six of row 0x28's and
# 0x7C38, and both have characters in row 0x2B.
ISO_IR_165 = ("ISO-IR-165EXT%UCS.src", f"{CITRUS}/GB")
GB_1988 = ("ISO646-CN%UCS.646", f"{CITRUS}/ISO646")

# BIG5, which glibc also calls CP950, is Microsoft's CP950. Where several codes
# read as the same character, it is written as Microsoft's best fit table gives,
# except that glibc writes the four doubled box drawing lines at their codes in
# row 0xA2, as the other box drawing characters are. glibc also reads the codes
# from 0xC6A1 to 0xC8FE, which Microsoft's table leaves out, as private use
# characters from U+F6B1, as Windows does.
CP950 = ("CP950.TXT", "bestfit950.txt", f"{UNICODE}/VENDORS/MICSFT")
BIG5_ROW_A2_WRITES = [0x2550, 0x255E, 0x2561, 0x256A]
# BIG5-HKSCS is Big5 as the Unicode Consortium's table has it, with the 2008
# edition of the Hong Kong Supplementary Character Set, as Hong Kong's
# government publishes it, over it. HKSCS has its own characters from 0xC6A1 to
# 0xC8FE, so Big5's there are left out, as are the codes Big5's table gives no
# character. Where HKSCS has a character Big5 also has, it is written as
# HKSCS's code, as glibc does.
BIG5_TXT = ("BIG5.TXT", f"{EASTASIA}/OTHER")
HKSCS_2008 = ("hkscs-2008-big5-iso.txt", "https://www.ccli.gov.hk/doc")
# EUC-TW has the planes of CNS 11643 glibc has, 1 to 7 and 15, as the tables
# Taiwan's government publishes have them now. glibc's are older: these have
# more characters, read some punctuation of plane 1 otherwise, and read as
# unified ideographs some codes glibc reads as compatibility ideographs. The
# private use characters the tables give codes with no Unicode character are
# left out.
CNS_11643 = (
    [
        "CNS2UNICODE_Unicode BMP.txt",
        "CNS2UNICODE_Unicode 2.txt",
        "CNS2UNICODE_Unicode 3.txt",
    ],
    "https://www.cns11643.gov.tw/opendata/MapingTables.zip",
)
CNS_PLANES = [1, 2, 3, 4, 5, 6, 7, 15]

# The Korean sets share KS X 1001, from the Unicode Consortium's KSC5601 table,
# whose codes with both bytes from 0xA1 are KS X 1001's. KS X 1001:1998 added
# the euro and registered signs, which CP949 has, and KS X 1001:2002 a postal
# code mark. CP949 writes the Hangul syllables KS X 1001 lacks in order after
# it, and JOHAB builds its syllables from their letters, so neither needs a
# table of its own.
KSC5601 = ("KSC5601.TXT", f"{EASTASIA}/KSC")
KS_X_1001_ADDITIONS = {0x2266: 0x20AC, 0x2267: 0x00AE, 0x2268: 0x327E}
CP949 = ("CP949.TXT", f"{UNICODE}/VENDORS/MICSFT/WINDOWS")
JOHAB = ("JOHAB.TXT", f"{EASTASIA}/KSC")

# The value a table holds for a byte the set does not assign.
UNASSIGNED = 0xFFFD

# The value a table holds for a byte which stands for several characters.
MULTIPLE = 0xFFFF


def read_unicode_mapping(path: str) -> dict[int, int]:
    """Reads a Unicode Consortium mapping file: a byte, a code point, and a
    comment, one to a line."""
    mapping = {}
    with open(path, encoding="latin-1") as file:
        for line in file:
            fields = line.split("#")[0].split()
            if len(fields) < 2 or not fields[0].startswith("0x"):
                continue
            if not fields[1].startswith("0x"):
                continue  # A byte the set leaves unassigned.
            mapping[int(fields[0], 16)] = int(fields[1], 16)
    return mapping


def read_citrus_mapping(path: str) -> dict[int, int]:
    """Reads a Citrus csmapper source: "0xAA = 0xUUUU" for one byte, and
    "0xAA - 0xBB = 0xUUUU -" for a range mapped in step."""
    mapping = {}
    in_map = False
    with open(path, encoding="latin-1") as file:
        for line in file:
            fields = line.split("#")[0].split()
            if fields[:1] == ["BEGIN_MAP"]:
                in_map = True
                continue
            if fields[:1] == ["END_MAP"] or not in_map or not fields:
                continue
            if len(fields) >= 5 and fields[1] == "-" and fields[3] == "=":
                first, last = int(fields[0], 16), int(fields[2], 16)
                target = int(fields[4], 16)
                stepped = len(fields) >= 6 and fields[5] == "-"
                for byte in range(first, last + 1):
                    mapping[byte] = target + (byte - first if stepped else 0)
            elif len(fields) >= 3 and fields[1] == "=":
                value = int(fields[2], 16)
                if value != 0xFFFE:  # Citrus marks an unassigned byte so.
                    mapping[int(fields[0], 16)] = value
    return mapping


def read_ucm_mapping(path: str) -> dict[int, int]:
    """Reads the bytes to code points an ICU ucm file gives: its round trip
    mappings, and those it only reads."""
    mapping = {}
    with open(path, encoding="latin-1") as file:
        for line in file:
            fields = line.split()
            if len(fields) < 3 or not fields[0].startswith("<U"):
                continue
            if fields[2] not in ("|0", "|3") or not fields[1].startswith("\\x"):
                continue
            mapping[int(fields[1][2:], 16)] = int(fields[0][2:-1], 16)
    return mapping


def read_iso646_mapping(path: str) -> dict[int, int]:
    """Reads a Citrus ISO 646 variant: the code points of the twelve national
    positions, in order, over ASCII."""
    positions = [0x23, 0x24, 0x40, 0x5B, 0x5C, 0x5D, 0x5E, 0x60, 0x7B, 0x7C, 0x7D, 0x7E]
    with open(path, encoding="latin-1") as file:
        values = [int(line.split()[0], 16) for line in file if line.startswith("0x")]
    if len(values) != len(positions):
        exit(f"{path}: expected {len(positions)} positions, found {len(values)}")
    mapping = {byte: byte for byte in range(0x80)}
    mapping.update(zip(positions, values))
    return mapping


def read_apple_mapping(path: str) -> dict:
    """Reads one of Apple's tables, which may mark a character with the
    direction it is written in, as <LR>+0x0020, and may give a byte as several
    characters, as 0x05F2+0x05B7. Those are a tuple."""
    mapping = {}
    with open(path, encoding="latin-1") as file:
        for line in file:
            fields = line.split("#")[0].split()
            if len(fields) < 2 or not fields[0].startswith("0x"):
                continue
            code_points = tuple(
                int(part, 16) for part in fields[1].split("+") if part.startswith("0x")
            )
            mapping[int(fields[0], 16)] = (
                code_points[0] if len(code_points) == 1 else code_points
            )
    return mapping


def read_mapping(path: str) -> dict[int, int]:
    if "/APPLE-" in path:
        return read_apple_mapping(path)
    if path.endswith(".646"):
        return read_iso646_mapping(path)
    if path.endswith(".src"):
        return read_citrus_mapping(path)
    if path.endswith(".ucm"):
        return read_ucm_mapping(path)
    return read_unicode_mapping(path)


def read_unicode_data(path: str) -> tuple[dict[int, int], dict[int, list[int]]]:
    """Reads each character's canonical combining class, where it is not 0,
    and its canonical decomposition, where it has one."""
    classes, decompositions = {}, {}
    with open(path, encoding="utf-8") as file:
        for line in file:
            fields = line.split(";")
            code_point = int(fields[0], 16)
            if fields[3] != "0":
                classes[code_point] = int(fields[3])
            if fields[5] and not fields[5].startswith("<"):
                decompositions[code_point] = [
                    int(part, 16) for part in fields[5].split()
                ]
    return classes, decompositions


def decompose(code_points, classes, decompositions) -> tuple[int, ...]:
    """The canonical decomposition of a string, with its marks in canonical
    order."""
    result = []
    for code_point in code_points:
        if code_point in decompositions:
            result += decompose(decompositions[code_point], classes, decompositions)
        else:
            result.append(code_point)
    for end in range(len(result), 1, -1):
        for i in range(1, end):
            if 0 < classes.get(result[i], 0) < classes.get(result[i - 1], 0):
                result[i - 1], result[i] = result[i], result[i - 1]
    return tuple(result)


def combining_tables(name: str, mapping: dict[int, int], classes, decompositions):
    """Works out which characters the set joins from a character and a mark,
    and the bytes each joined character the set has no byte for is written
    as."""
    byte_of = {}
    for byte in sorted(mapping):
        byte_of.setdefault(mapping[byte], byte)
    joined = {}
    for code_point, parts in decompositions.items():
        if len(parts) > 1:
            key = decompose([code_point], classes, decompositions)
            joined.setdefault(key, []).append(code_point)

    characters = sorted(byte_of)
    compositions = {}
    level = {code_point: 0 for code_point in characters}
    queue = list(characters)
    for held in queue:
        for mark in characters:
            found = joined.get(decompose([held, mark], classes, decompositions), [])
            if len(found) > 1:
                exit(f"{name}: U+{held:04X} U+{mark:04X} could join to more than one")
            if not found:
                continue
            compositions[(held, mark)] = found[0]
            if found[0] not in level:
                level[found[0]] = level[held] + 1
                queue.append(found[0])

    # The fewest bytes, and of those, marks in canonical order.
    def order(data: bytes):
        return len(data), [classes.get(mapping[byte], 0) for byte in data[1:]], data

    written = {code_point: bytes([byte_of[code_point]]) for code_point in characters}
    for code_point in sorted(level, key=lambda c: (level[c], c)):
        if code_point in written:
            continue
        candidates = [
            written[held] + bytes([byte_of[mark]])
            for (held, mark), result in compositions.items()
            if result == code_point and held in written
        ]
        written[code_point] = min(candidates, key=order)

    # Read each back the way iconv does, holding a character while a mark may
    # still join it.
    bases = {held for held, _ in compositions}

    def read(data: bytes) -> list[int]:
        out, held = [], None
        for byte in data:
            code_point = mapping[byte]
            if held is not None and (held, code_point) in compositions:
                code_point, held = compositions[(held, code_point)], None
            elif held is not None:
                out.append(held)
                held = None
            if code_point in bases:
                held = code_point
            else:
                out.append(code_point)
        return out + ([held] if held is not None else [])

    for code_point, data in written.items():
        if read(data) != [code_point] or len(data) > 3 or code_point > 0xFFFF:
            exit(f"{name}: U+{code_point:04X} is not written as {data.hex()}")
    if 0 in bases:
        exit(f"{name}: U+0000 cannot be held")
    decompositions_out = sorted(
        (code_point, data) for code_point, data in written.items() if len(data) > 1
    )
    return sorted(compositions.items()), decompositions_out


def format_combining(name: str, compositions, decompositions) -> str:
    lines = [f"constexpr Composition {name}_COMPOSITIONS[{len(compositions)}] = {{"]
    for start in range(0, len(compositions), 2):
        row = compositions[start : start + 2]
        lines.append(
            "    "
            + " ".join(
                f"{{0x{held:04X}, 0x{mark:04X}, 0x{result:04X}}},"
                for (held, mark), result in row
            )
        )
    lines.append("};")
    lines.append("")
    lines.append(
        f"constexpr Decomposition {name}_DECOMPOSITIONS[{len(decompositions)}] = {{"
    )
    for start in range(0, len(decompositions), 2):
        row = decompositions[start : start + 2]
        entries = []
        for code_point, data in row:
            padded = list(data) + [0] * (3 - len(data))
            entries.append(
                f"{{0x{code_point:04X}, {len(data)}, {{"
                + ", ".join(f"0x{byte:02X}" for byte in padded)
                + "}},"
            )
        lines.append("    " + " ".join(entries))
    lines.append("};")
    lines.append("")
    lines.append(f"constexpr Combining {name}_COMBINING = {{")
    lines.append(f"    {name}_COMPOSITIONS, {len(compositions)},")
    lines.append(f"    {name}_DECOMPOSITIONS, {len(decompositions)},")
    lines.append("};")
    return "\n".join(lines)


def split_sequences(mapping: dict) -> dict[int, tuple[int, ...]]:
    """Takes the bytes which stand for several characters out of |mapping|,
    leaving MULTIPLE in their place."""
    sequences = {
        byte: value for byte, value in mapping.items() if isinstance(value, tuple)
    }
    for byte in sequences:
        mapping[byte] = MULTIPLE
    return sequences


def format_sequences(name: str, sequences: dict[int, tuple[int, ...]]) -> str:
    lines = [f"constexpr Sequence {name}_SEQUENCE_LIST[{len(sequences)}] = {{"]
    for code, code_points in sorted(sequences.items()):
        if len(code_points) > 3 or max(code_points) > 0xFFFF:
            exit(f"{name}: 0x{code:02X} stands for more than a sequence holds")
        padded = list(code_points) + [0] * (3 - len(code_points))
        lines.append(
            f"    {{0x{code:02X}, {len(code_points)}, {{"
            + ", ".join(f"0x{code_point:04X}" for code_point in padded)
            + "}},"
        )
    lines.append("};")
    lines.append("")
    lines.append(
        f"constexpr Sequences {name}_SEQUENCES = {{{name}_SEQUENCE_LIST, "
        + f"{len(sequences)}}};"
    )
    return "\n".join(lines)


def read_columns(path: str, key: int, value: int) -> dict[int, int]:
    """Reads two columns of a Unicode Consortium mapping file."""
    mapping = {}
    with open(path, encoding="latin-1") as file:
        for line in file:
            fields = line.split("#")[0].split()
            if len(fields) <= max(key, value):
                continue
            if fields[key].startswith("0x") and fields[value].startswith("0x"):
                mapping[int(fields[key], 16)] = int(fields[value], 16)
    return mapping


def read_best_fit_writes(path: str) -> dict[int, int]:
    """Reads the code Microsoft's best fit table writes each code point as."""
    writes, in_table = {}, False
    with open(path, encoding="latin-1") as file:
        for line in file:
            fields = line.split(";")[0].split()
            if not fields:
                continue
            if not fields[0].startswith("0x"):
                in_table = fields[0] == "WCTABLE"
            elif in_table and len(fields) >= 2:
                writes[int(fields[0], 16)] = int(fields[1], 16)
    return writes


def shift_jis_from_jis(code: int) -> int:
    row, column = (code >> 8) - 0x21, (code & 0xFF) - 0x21
    lead = (row >> 1) + (0x81 if row < 62 else 0xC1)
    if row & 1:
        trail = column + 0x9F
    else:
        trail = column + 0x40 + (1 if column >= 0x3F else 0)
    return lead << 8 | trail


def format_values(kind: str, name: str, values: list[int], digits: int) -> str:
    per_line = 72 // (digits + 4)
    lines = [f"const {kind} {name}[{len(values)}] = {{"]
    for start in range(0, len(values), per_line):
        row = values[start : start + per_line]
        lines.append("    " + " ".join(f"0x{value:0{digits}X}," for value in row))
    lines.append("};")
    return "\n".join(lines)


def format_code_table(
    name: str,
    mapping: dict[int, int],
    leads: tuple[int, int],
    trails: tuple[int, int],
    writes: dict[int, int] | None = None,
) -> str:
    """A CodeTable of the codes from |leads| and |trails|, each an inclusive
    range. With |writes|, only a character written as its code here is in the
    order, which is how writing finds it."""
    lead_count = leads[1] - leads[0] + 1
    trail_count = trails[1] - trails[0] + 1
    cells = [0] * (lead_count * trail_count)
    for code, code_point in mapping.items():
        row, column = (code >> 8) - leads[0], (code & 0xFF) - trails[0]
        if not (0 <= row < lead_count and 0 <= column < trail_count):
            exit(f"{name}: 0x{code:04X} is outside the table")
        if not 0 < code_point <= 0xFFFF:
            exit(f"{name}: 0x{code:04X} is U+{code_point:04X}, which does not fit")
        cells[row * trail_count + column] = code_point
    order, written = [], set()
    for position in sorted(range(len(cells)), key=lambda p: (cells[p], p)):
        code_point = cells[position]
        code = (leads[0] + position // trail_count) << 8
        code |= trails[0] + position % trail_count
        if code_point == 0:
            continue
        if writes is not None and writes.get(code_point) != code:
            continue
        if code_point in written:
            exit(f"{name}: U+{code_point:04X} is written as two codes")
        written.add(code_point)
        order.append(position)
    arrays = [format_values("uint16_t", f"{name}_CODE_POINTS", cells, 4)]
    order_name = "nullptr"
    if order:
        arrays.append(format_values("uint16_t", f"{name}_ORDER", order, 4))
        order_name = f"{name}_ORDER"
    return "\n".join(
        ["namespace {"]
        + arrays
        + [
            "} // namespace",
            "",
            f"const CodeTable {name} = {{",
            f"    0x{leads[0]:02X}, {lead_count}, 0x{trails[0]:02X}, {trail_count},",
            f"    {name}_CODE_POINTS,",
            f"    {order_name}, {len(order)},",
            "};",
        ]
    )


def read_ucm_entries(path: str) -> list[tuple[int, int, str]]:
    """Reads each mapping of an ICU ucm file: the code point, the bytes as one
    number, and the kind of mapping after the "|"."""
    entries = []
    with open(path, encoding="latin-1") as file:
        for line in file:
            match = re.match(r"<U([0-9A-F]+)>\s+((?:\\x[0-9A-F]{2})+)\s+\|(\d)", line)
            if match:
                code = int(match.group(2).replace("\\x", ""), 16)
                entries.append((int(match.group(1), 16), code, match.group(3)))
    return entries


def gb18030_index(code: int) -> int:
    """The position of a four byte GB18030 code among all of them."""
    first, second, third, fourth = code.to_bytes(4, "big")
    return (((first - 0x81) * 10 + second - 0x30) * 126 + third - 0x81) * 10 + (
        fourth - 0x30
    )


def code_ranges(codes: list[int]) -> list[tuple[int, int]]:
    ranges = []
    for code in sorted(codes):
        if ranges and code == ranges[-1][1] + 1:
            ranges[-1] = (ranges[-1][0], code)
        else:
            ranges.append((code, code))
    return ranges


def chinese_tables(mappings: str) -> tuple[list[str], list[str]]:
    """The definitions and declarations of the Simplified Chinese sets' tables."""
    two, four, read_only, write_only = {}, {}, [], {}
    for code_point, code, kind in read_ucm_entries(f"{mappings}/{GB18030[0]}"):
        if kind == "0" and code > 0xFF:
            (two if code <= 0xFFFF else four)[code] = code_point
        elif kind == "3":
            read_only.append((code_point, code))
        elif kind == "1":
            write_only[code] = code_point
    # ICU reads 0xA3A0 as U+3000, and writes U+E5E5 as it. glibc reads and
    # writes it as U+E5E5.
    two[0xA3A0] = 0xE5E5
    # GB18030-2022 gave 18 characters two byte codes of their own, and the
    # private use characters those codes had took the characters' four byte
    # codes. ICU still reads the four byte codes as the characters; glibc reads
    # them as the private use characters, as the standard has it.
    by_code_point = {code_point: code for code, code_point in two.items()}
    for code_point, code in read_only:
        if code > 0xFFFF:
            four[code] = write_only[by_code_point[code_point]]
    # Every other four byte code in the Basic Multilingual Plane stands for the
    # next code point which has no code yet.
    pinned = {gb18030_index(c): cp for c, cp in four.items() if c <= 0x8439FE39}
    taken = set(two.values()) | set(pinned.values())
    free = [
        code_point
        for code_point in range(0x80, 0x10000)
        if code_point not in taken and not 0xD800 <= code_point <= 0xDFFF
    ]
    count = len(pinned) + len(free)
    if max(pinned) >= count:
        exit("GB18030: a four byte code is beyond the Basic Multilingual Plane's")
    free.reverse()
    positions = [pinned[i] if i in pinned else free.pop() for i in range(count)]
    runs = []
    for position, code_point in enumerate(positions):
        if runs and code_point == runs[-1][1] + position - runs[-1][0]:
            continue
        runs.append((position, code_point))
    if len(runs) > 255:
        exit("GB18030: too many runs of four byte codes")
    by_code_point_order = sorted(range(len(runs)), key=lambda i: runs[i][1])

    definitions = [
        format_code_table("GB18030_TWO_BYTE", two, (0x81, 0xFE), (0x40, 0xFE)),
        "const CodeRun GB18030_RUNS[GB18030_RUN_COUNT] = {\n"
        + "\n".join(f"    {{{p}, 0x{cp:04X}}}," for p, cp in runs)
        + "\n};",
        format_values("uint8_t", "GB18030_RUNS_BY_CODE_POINT", by_code_point_order, 2),
    ]
    declarations = [
        "extern const CodeTable GB18030_TWO_BYTE;",
        f"constexpr size_t GB18030_RUN_COUNT = {len(runs)};",
        f"constexpr uint32_t GB18030_FOUR_BYTE_COUNT = {count};",
        "// Where a run of consecutive code points begins among the four byte",
        "// codes, in order of position.",
        "extern const CodeRun GB18030_RUNS[GB18030_RUN_COUNT];",
        "extern const uint8_t GB18030_RUNS_BY_CODE_POINT[GB18030_RUN_COUNT];",
    ]

    cp936 = read_columns(f"{mappings}/{CP936[0]}", 0, 1)
    if cp936.get(0x80) != 0x20AC:
        exit("CP936: 0x80 is not the euro sign")
    gbk = {code: cp for code, cp in cp936.items() if code > 0xFF}
    if any(two.get(code) != cp or 0xE000 <= cp <= 0xF8FF for code, cp in gbk.items()):
        exit("CP936: a two byte code is not GB18030's")
    # GBK has no private use characters, nor these codes of GB18030's.
    excluded = [
        code
        for code, cp in two.items()
        if code not in gbk and not 0xE000 <= cp <= 0xF8FF
    ]
    ranges = code_ranges(excluded)
    definitions.append(
        "const CodeRange GBK_EXCLUDED[GBK_EXCLUDED_COUNT] = {\n"
        + "\n".join(f"    {{0x{a:04X}, 0x{b:04X}}}," for a, b in ranges)
        + "\n};"
    )
    declarations.append(f"constexpr size_t GBK_EXCLUDED_COUNT = {len(ranges)};")
    declarations.append("extern const CodeRange GBK_EXCLUDED[GBK_EXCLUDED_COUNT];")

    gb2312 = read_citrus_mapping(f"{mappings}/{GB2312[0]}")
    gb2312 = {code: cp for code, cp in gb2312.items() if code > 0xFF}
    assigned = [0] * ((94 * 94 + 7) // 8)
    changes = []
    for code, code_point in sorted(gb2312.items()):
        if code | 0x8080 not in two:
            exit(f"GB 2312: 0x{code:04X} is not a GB18030 code")
        if two[code | 0x8080] != code_point:
            changes.append((code, code_point))
        bit = ((code >> 8) - 0x21) * 94 + (code & 0xFF) - 0x21
        assigned[bit >> 3] |= 1 << (bit & 7)
    definitions.append(format_values("uint8_t", "GB2312_ASSIGNED", assigned, 2))
    definitions.append(
        "const CodePair GB2312_CHANGES[GB2312_CHANGE_COUNT] = {\n"
        + "\n".join(f"    {{0x{c:04X}, 0x{cp:04X}}}," for c, cp in changes)
        + "\n};"
    )
    declarations.append("// A bit for each GB 2312 code with a character, row by row.")
    declarations.append(f"extern const uint8_t GB2312_ASSIGNED[{len(assigned)}];")
    declarations.append("// The GB 2312 codes GB18030 reads as other characters.")
    declarations.append(f"constexpr size_t GB2312_CHANGE_COUNT = {len(changes)};")
    declarations.append("extern const CodePair GB2312_CHANGES[GB2312_CHANGE_COUNT];")

    additions = read_citrus_mapping(f"{mappings}/{ISO_IR_165[0]}")
    if additions.get(0x283C) != 0xE7C7:
        exit("ISO-IR-165: 0x283C is not the private use character")
    additions[0x283C] = 0x1E3F
    gb_1988 = read_iso646_mapping(f"{mappings}/{GB_1988[0]}")
    for byte in range(0x21, 0x7F):
        additions[0x2A00 | byte] = gb_1988.get(byte, byte)
    if any(code in gb2312 for code in additions):
        exit("ISO-IR-165: an addition has a GB 2312 code")
    arrays, declaration = format_code_list("ISO_IR_165_ADDITIONS", additions)
    definitions.append(arrays)
    declarations.append("// The characters ISO-IR-165 has beyond GB 2312.")
    declarations.append(declaration)
    return definitions, declarations


JOHAB_MEDIALS = [3, 4, 5, 6, 7, 10, 11, 12, 13, 14, 15, 18, 19, 20, 21, 22, 23]
JOHAB_MEDIALS += [26, 27, 28, 29]
JOHAB_FINALS = [1] + list(range(2, 18)) + list(range(19, 30))


def johab_syllable(code: int) -> int:
    """The Hangul syllable a JOHAB code builds, or 0."""
    initial, medial, final = (code >> 10) & 31, (code >> 5) & 31, code & 31
    if not code & 0x8000 or not 2 <= initial <= 20:
        return 0
    if medial not in JOHAB_MEDIALS or final not in JOHAB_FINALS:
        return 0
    return (
        0xAC00
        + ((initial - 2) * 21 + JOHAB_MEDIALS.index(medial)) * 28
        + (JOHAB_FINALS.index(final))
    )


def johab_from_ks_x_1001(code: int) -> int:
    """Where JOHAB has a KS X 1001 symbol or hanja, or 0 for its Hangul."""
    row, column = (code >> 8) - 0x20, (code & 0xFF) - 0x20
    if row <= 12:
        lead, trail_row = 0xD8 + (row + 1) // 2, row % 2 == 1
    elif row >= 42:
        lead, trail_row = 0xE0 + (row - 42) // 2, row % 2 == 0
    else:
        return 0
    if not trail_row:
        return lead << 8 | (column + 0xA0)
    return lead << 8 | (column + (0x30 if column <= 0x4E else 0x42))


def format_code_list(name: str, mapping: dict[int, int]) -> tuple[str, str]:
    """A CodeList of |mapping|, whose values fit in 16 bits and differ, and its
    declaration."""
    pairs = sorted(mapping.items())
    if any(not 0 <= value <= 0xFFFF for _, value in pairs):
        exit(f"{name}: a value does not fit")
    if len({value for _, value in pairs}) != len(pairs):
        exit(f"{name}: a value has two codes")
    order = sorted(range(len(pairs)), key=lambda i: pairs[i][1])
    arrays = "\n".join(
        [
            "namespace {",
            f"const CodePair {name}_PAIRS[{len(pairs)}] = {{",
            "\n".join(f"    {{0x{c:04X}, 0x{v:04X}}}," for c, v in pairs),
            "};",
            format_values("uint16_t", f"{name}_ORDER", order, 4),
            "} // namespace",
            "",
            f"const CodeList {name} = {{{name}_PAIRS, {name}_ORDER, {len(pairs)}}};",
        ]
    )
    return arrays, f"extern const CodeList {name};"


def format_wide_table(name: str, mapping: dict[int, int]) -> tuple[str, str]:
    """The arrays of a WideTable of the codes from 0x2121 to 0x7E7E, and its
    initializer."""
    cells = [0] * (94 * 94)
    for code, code_point in mapping.items():
        row, column = (code >> 8) - 0x21, (code & 0xFF) - 0x21
        if not (0 <= row < 94 and 0 <= column < 94):
            exit(f"{name}: 0x{code:04X} is outside the table")
        if not 0 < code_point <= 0x3FFFF:
            exit(f"{name}: 0x{code:04X} is U+{code_point:04X}, which does not fit")
        cells[row * 94 + column] = code_point
    planes = [0] * (len(cells) // 4 + 1)
    for position, code_point in enumerate(cells):
        planes[position // 4] |= (code_point >> 16) << (position % 4 * 2)
    order = sorted((p for p, cp in enumerate(cells) if cp), key=lambda p: cells[p])
    if len({cells[p] for p in order}) != len(order):
        exit(f"{name}: a character has two codes")
    arrays = "\n".join(
        [
            "namespace {",
            format_values(
                "uint16_t", f"{name}_CODE_POINTS", [c & 0xFFFF for c in cells], 4
            ),
            format_values("uint8_t", f"{name}_PLANES", planes, 2),
            format_values("uint16_t", f"{name}_ORDER", order, 4),
            "} // namespace",
        ]
    )
    initializer = f"{{{name}_CODE_POINTS, {name}_PLANES, {name}_ORDER, {len(order)}}}"
    return arrays, initializer


def traditional_chinese_tables(mappings: str) -> tuple[list[str], list[str]]:
    """The definitions and declarations of the Traditional Chinese sets' tables."""
    table_file, best_fit_file, _ = CP950
    cp950 = read_columns(f"{mappings}/{table_file}", 0, 1)
    big5 = {code: cp for code, cp in cp950.items() if code > 0xFF}
    writes = read_best_fit_writes(f"{mappings}/{best_fit_file}")
    for code_point in BIG5_ROW_A2_WRITES:
        codes = [code for code, cp in big5.items() if cp == code_point]
        if len(codes) != 2 or min(codes) >> 8 != 0xA2:
            exit(f"CP950: U+{code_point:04X} is not in row 0xA2 and another")
        writes[code_point] = min(codes)
    trails = list(range(0x40, 0x7F)) + list(range(0xA1, 0xFF))
    private_use = [lead << 8 | trail for lead in (0xC6, 0xC7, 0xC8) for trail in trails]
    for index, code in enumerate(c for c in private_use if c >= 0xC6A1):
        if code in big5:
            exit(f"CP950: 0x{code:04X} has a character")
        big5[code] = 0xF6B1 + index
        writes[0xF6B1 + index] = code
    definitions = [
        format_code_table("BIG5_TWO_BYTE", big5, (0xA1, 0xF9), (0x40, 0xFE), writes)
    ]
    declarations = ["extern const CodeTable BIG5_TWO_BYTE;"]

    hkscs = {
        code: cp
        for code, cp in read_columns(f"{mappings}/{BIG5_TXT[0]}", 0, 1).items()
        if code > 0xFF and cp != 0xFFFD and not 0xC6A1 <= code <= 0xC8FE
    }
    own, sequences, plane_2 = set(), {}, {}
    with open(f"{mappings}/{HKSCS_2008[0]}", encoding="utf-8-sig") as file:
        for line in file:
            fields = line.split()
            if len(fields) < 2 or not re.fullmatch(r"[0-9A-F]{4}", fields[0]):
                continue
            # The last column has the characters in ISO/IEC 10646:2003 with its
            # amendments, and several of them between angle brackets.
            code = int(fields[0], 16)
            code_points = [int(cp, 16) for cp in fields[-1].strip("<>").split(",")]
            own.add(code)
            hkscs.pop(code, None)
            if len(code_points) > 1:
                sequences[code] = tuple(code_points)
            elif code_points[0] > 0xFFFF:
                plane_2[code] = code_points[0]
            else:
                hkscs[code] = code_points[0]
    hkscs_writes = {}
    for code in sorted(hkscs, key=lambda code: (code not in own, code)):
        hkscs_writes.setdefault(hkscs[code], code)
    if any(not 0x20000 <= cp <= 0x2FFFF for cp in plane_2.values()):
        exit("HKSCS: a character is beyond the Supplementary Ideographic Plane")
    if len(set(plane_2.values())) != len(plane_2):
        exit("HKSCS: a character of the Supplementary Ideographic Plane has two codes")
    plane_2_arrays, plane_2_declaration = format_code_list(
        "BIG5_HKSCS_PLANE_2", {code: cp - 0x20000 for code, cp in plane_2.items()}
    )
    definitions += [
        format_code_table(
            "BIG5_HKSCS", hkscs, (0x87, 0xFE), (0x40, 0xFE), hkscs_writes
        ),
        plane_2_arrays,
    ]
    declarations += [
        "extern const CodeTable BIG5_HKSCS;",
        "// The characters of the Supplementary Ideographic Plane, as their",
        "// offset from U+20000.",
        plane_2_declaration,
        format_sequences("BIG5_HKSCS", sequences),
    ]

    cns = {plane: {} for plane in CNS_PLANES}
    file_names, _ = CNS_11643
    for file_name in file_names:
        with open(f"{mappings}/{file_name}", encoding="utf-8-sig") as file:
            for line in file:
                match = re.match(r"(\d+)-([0-9A-F]{4})\s+([0-9A-F]{4,5})\s*$", line)
                if match and int(match.group(1)) in cns:
                    code, code_point = int(match.group(2), 16), int(match.group(3), 16)
                    cns[int(match.group(1))][code] = code_point
    initializers = []
    for plane, mapping in cns.items():
        arrays, initializer = format_wide_table(f"CNS_11643_{plane}", mapping)
        definitions.append(arrays)
        initializers.append(f"    {initializer},")
    definitions.append(
        "const WideTable CNS_11643[CNS_PLANE_COUNT] = {\n"
        + "\n".join(initializers)
        + "\n};"
    )
    declarations += [
        f"constexpr size_t CNS_PLANE_COUNT = {len(CNS_PLANES)};",
        "// Planes "
        + ", ".join(str(p) for p in CNS_PLANES[:-1])
        + f" and {CNS_PLANES[-1]} of CNS 11643.",
        "extern const WideTable CNS_11643[CNS_PLANE_COUNT];",
    ]
    return definitions, declarations


def korean_tables(mappings: str) -> tuple[list[str], list[str]]:
    """The definitions and declarations of the Korean sets' tables."""
    ksc5601 = read_columns(f"{mappings}/{KSC5601[0]}", 0, 1)
    ks_x_1001 = {
        code & 0x7F7F: cp
        for code, cp in ksc5601.items()
        if code >> 8 >= 0xA1 and code & 0xFF >= 0xA1
    }
    ks_x_1001.update(KS_X_1001_ADDITIONS)

    # CP949: KS X 1001 without the postal code mark, and the other Hangul
    # syllables in order.
    in_ks_x_1001 = set(ks_x_1001.values())
    trails = list(range(0x41, 0x5B)) + list(range(0x61, 0x7B)) + list(range(0x81, 0xFF))
    extra_codes = [
        lead << 8 | trail
        for lead in range(0x81, 0xFF)
        for trail in trails
        if lead < 0xA1 or trail < 0xA1
    ]
    extra = [cp for cp in range(0xAC00, 0xD7A4) if cp not in in_ks_x_1001]
    model = {code | 0x8080: cp for code, cp in ks_x_1001.items() if code != 0x2268}
    model.update(zip(extra_codes, extra))
    cp949 = read_columns(f"{mappings}/{CP949[0]}", 0, 1)
    if model != {code: cp for code, cp in cp949.items() if code > 0xFF}:
        exit("CP949: KS X 1001 and the Hangul syllables after it are not CP949")

    # JOHAB: syllables from their letters, the Hangul letters KS X 1001 has
    # first as codes with only one letter, and its other symbols and hanja
    # moved. Unicode's JOHAB table is older than the additions, which glibc
    # and GNU libiconv both have.
    johab = {
        code: cp
        for code, cp in read_columns(f"{mappings}/{JOHAB[0]}", 0, 1).items()
        if code > 0xFF
    }
    johab.update({johab_from_ks_x_1001(k): cp for k, cp in KS_X_1001_ADDITIONS.items()})
    letters = sorted(
        (code, cp)
        for code, cp in johab.items()
        if code >> 8 < 0xD8 and johab_syllable(code) != cp
    )
    letter_code_points = {cp for _, cp in letters}
    built = {c: johab_syllable(c) for c in range(0x8000, 0x10000) if johab_syllable(c)}
    built.update(letters)
    for k, cp in ks_x_1001.items():
        if johab_from_ks_x_1001(k) and cp not in letter_code_points:
            built[johab_from_ks_x_1001(k)] = cp
    if built != johab:
        exit("JOHAB: the rules do not build JOHAB's table")
    if any((k >> 8) != 0x24 for k, cp in ks_x_1001.items() if cp in letter_code_points):
        exit("JOHAB: a letter is not in KS X 1001's row of letters")

    definitions = [
        format_code_table("KS_X_1001", ks_x_1001, (0x21, 0x7E), (0x21, 0x7E)),
        "const CodePair JOHAB_LETTERS[JOHAB_LETTER_COUNT] = {\n"
        + "\n".join(f"    {{0x{c:04X}, 0x{cp:04X}}}," for c, cp in letters)
        + "\n};",
    ]
    declarations = [
        "extern const CodeTable KS_X_1001;",
        "// The JOHAB codes of a Hangul letter by itself.",
        f"constexpr size_t JOHAB_LETTER_COUNT = {len(letters)};",
        "extern const CodePair JOHAB_LETTERS[JOHAB_LETTER_COUNT];",
    ]
    return definitions, declarations


def japanese_tables(mappings: str) -> tuple[list[str], list[str]]:
    """The definitions and declarations of the Japanese sets' tables."""
    file_name, _, changes = JIS_X0208
    jis_x0208 = read_columns(f"{mappings}/{file_name}", 1, 2)
    jis_x0208.update(changes)
    file_name, _, changes = JIS_X0212
    jis_x0212 = read_columns(f"{mappings}/{file_name}", 0, 1)
    jis_x0212.update(changes)
    definitions = [
        format_code_table("JIS_X0208", jis_x0208, (0x21, 0x7E), (0x21, 0x7E)),
        format_code_table("JIS_X0212", jis_x0212, (0x21, 0x7E), (0x21, 0x7E)),
    ]
    declarations = [
        "extern const CodeTable JIS_X0208;",
        "extern const CodeTable JIS_X0212;",
    ]

    table_file, best_fit_file, _ = CP932
    cp932 = read_columns(f"{mappings}/{table_file}", 0, 1)
    cp932 = {code: code_point for code, code_point in cp932.items() if code > 0xFF}
    writes = read_best_fit_writes(f"{mappings}/{best_fit_file}")
    from_jis = {shift_jis_from_jis(code): cp for code, cp in jis_x0208.items()}
    jis_leads = {code >> 8 for code in from_jis}
    if any(code not in cp932 for code in from_jis):
        exit("CP932: a JIS X 0208 code is missing")
    # The codes of JIS X 0208 which CP932 reads as another character.
    changes = sorted(
        (code, cp)
        for code, cp in cp932.items()
        if code in from_jis and from_jis[code] != cp
    )
    blocks = [
        ("CP932_NEC_ROW_13", (0x87, 0x87)),
        ("CP932_IBM", (0xFA, 0xFC)),
        ("CP932_NEC_SELECTED_IBM", (0xED, 0xEE)),
    ]
    extension = {code: cp for code, cp in cp932.items() if code not in from_jis}
    # ISO-2022-JP-MS writes characters of the NEC-selected block as its own
    # codes, so all of them are in its order. CP932 finds each one sooner.
    unfiltered = "CP932_NEC_SELECTED_IBM"
    for name, (first, last) in blocks:
        block = {c: cp for c, cp in extension.items() if first <= c >> 8 <= last}
        only = None if name == unfiltered else writes
        definitions.append(
            format_code_table(name, block, (first, last), (0x40, 0xFC), only)
        )
        declarations.append(f"extern const CodeTable {name};")
    if any(not any(f <= c >> 8 <= l for _, (f, l) in blocks) for c in extension):
        exit("CP932: a code is outside JIS X 0208 and the extension blocks")

    # Writing tries the changed codes, JIS X 0208 and then the blocks in the
    # order above. Check that finds what Microsoft writes.
    by_jis = {cp: code for code, cp in sorted(from_jis.items(), reverse=True)}
    for code_point, code in writes.items():
        if cp932.get(code) != code_point:
            continue
        found = dict((cp, c) for c, cp in changes).get(code_point)
        found = found or by_jis.get(code_point)
        for name, (first, last) in blocks:
            if found:
                break
            candidates = [c for c, cp in extension.items() if cp == code_point]
            candidates = [c for c in candidates if first <= c >> 8 <= last]
            if name == unfiltered:
                found = candidates[0] if candidates else None
            else:
                found = code if code in candidates else None
        if found != code:
            exit(f"CP932: U+{code_point:04X} would not be written as 0x{code:04X}")

    definitions.append(
        "const CodePair CP932_CHANGES[CP932_CHANGE_COUNT] = {\n"
        + "\n".join(f"    {{0x{code:04X}, 0x{cp:04X}}}," for code, cp in changes)
        + "\n};"
    )
    declarations.append(f"constexpr size_t CP932_CHANGE_COUNT = {len(changes)};")
    declarations.append("extern const CodePair CP932_CHANGES[CP932_CHANGE_COUNT];")
    # The areas CP932 is laid out in: JIS X 0208's rows of symbols and of
    # kanji, the extension blocks, and the user-defined area.
    symbols = [shift_jis_from_jis(c) for c in jis_x0208 if c >> 8 < 0x30]
    kanji = [shift_jis_from_jis(c) for c in jis_x0208 if c >> 8 >= 0x30]
    areas = [(min(symbols), max(symbols)), (min(kanji), max(kanji)), (0xF040, 0xF9FC)]
    for _, (first, last) in blocks:
        codes = [c for c in extension if first <= c >> 8 <= last]
        areas.append((min(codes), max(codes)))
    areas.sort()
    definitions.append(
        "const CodeRange CP932_AREAS[CP932_AREA_COUNT] = {\n"
        + "\n".join(f"    {{0x{first:04X}, 0x{last:04X}}}," for first, last in areas)
        + "\n};"
    )
    declarations.append(f"constexpr size_t CP932_AREA_COUNT = {len(areas)};")
    declarations.append("extern const CodeRange CP932_AREAS[CP932_AREA_COUNT];")

    # ISO-2022-JP-MS has the IBM extensions which neither JIS X 0208, as CP932
    # reads it, nor JIS X 0212 has in rows 0x73 and 0x74 of JIS X 0212, in
    # CP932's order. eucJP-ms, which has the same 106 from 0x8FF3F3, counts
    # U+FFE4 as JIS X 0212's broken bar and U+2116 as one of them. GNU libiconv
    # is the only implementation of ISO-2022-JP-MS, and this follows it: they
    # start at 0x7321, the ones in NEC's row 13 and U+663B are left out, and
    # U+974D is at 0x7463.
    in_jis_x0212 = (set(jis_x0212.values()) | {0xFFE4}) - {0x2116}
    elsewhere = {cp932[code] for code in from_jis} | in_jis_x0212
    row_13 = {cp for code, cp in extension.items() if code >> 8 == 0x87}
    ibm = [
        cp
        for code, cp in sorted(extension.items())
        if 0xFA <= code >> 8 <= 0xFC and cp not in elsewhere
    ]
    if len(ibm) != 106:
        exit(f"ISO-2022-JP-MS: {len(ibm)} IBM extensions, not 106")
    jp_ms_ibm = {}
    for index, cp in enumerate(ibm):
        if cp in row_13 or cp == 0x663B:
            continue
        code = (0x73 + index // 94) << 8 | (0x21 + index % 94)
        jp_ms_ibm[0x7463 if cp == 0x974D else code] = cp
    definitions.append(
        format_code_table("ISO2022_JP_MS_IBM", jp_ms_ibm, (0x73, 0x74), (0x21, 0x7E))
    )
    declarations.append("extern const CodeTable ISO2022_JP_MS_IBM;")
    return definitions, declarations


def high_half(name: str, mapping: dict[int, int]) -> list[int]:
    for byte in range(0x80):
        if mapping.get(byte, byte) != byte:
            exit(f"{name}: byte 0x{byte:02X} is not ASCII")
    return [mapping.get(byte, UNASSIGNED) for byte in range(0x80, 0x100)]


def format_table(name: str, values: list[int]) -> str:
    kind = "HIGH" if len(values) == 128 else "FULL"
    lines = [f"constexpr uint16_t {name}_{kind}[{len(values)}] = {{"]
    for start in range(0, len(values), 9):
        row = values[start : start + 9]
        lines.append("    " + " ".join(f"0x{value:04X}," for value in row))
    lines.append("};")
    return "\n".join(lines)


LICENSE = (
    "//\n"
    + "// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.\n"
    + "// See https://llvm.org/LICENSE.txt for license information.\n"
    + "// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception\n"
    + "//\n"
    + "//===----------------------------------------------------------------------===//\n"
)


def title_line(text: str, suffix: str) -> str:
    title = f"//===-- {text} "
    return title + "-" * (80 - len(title) - len(suffix)) + suffix


def write_cjk_tables(root: str, definitions: list[str], declarations: list[str]):
    notice = (
        "//\n"
        + "// DO NOT EDIT MANUALLY. This file is generated by\n"
        + "// libc/utils/iconv_utils/gen.py from the mapping files it lists.\n"
        + "//\n"
        + "//===----------------------------------------------------------------------===//\n"
    )
    with open(f"{root}/libc/src/iconv/cjk_tables.h", "w") as file:
        file.write(
            title_line("Tables for iconv's East Asian sets", "*- C++ -*-===//")
            + "\n"
            + LICENSE
            + notice
            + "\n"
            + "#ifndef LLVM_LIBC_SRC_ICONV_CJK_TABLES_H\n"
            + "#define LLVM_LIBC_SRC_ICONV_CJK_TABLES_H\n"
            + "\n"
            + '#include "hdr/stdint_proxy.h"\n'
            + '#include "hdr/types/size_t.h"\n'
            + '#include "src/__support/macros/config.h"\n'
            + '#include "src/iconv/code_table.h"\n'
            + '#include "src/iconv/sequences.h"\n'
            + "\n"
            + "namespace LIBC_NAMESPACE_DECL {\n"
            + "namespace iconv_internal {\n"
            + "\n"
            + "\n".join(declarations)
            + "\n\n"
            + "} // namespace iconv_internal\n"
            + "} // namespace LIBC_NAMESPACE_DECL\n"
            + "\n"
            + "#endif // LLVM_LIBC_SRC_ICONV_CJK_TABLES_H\n"
        )
    with open(f"{root}/libc/src/iconv/cjk_tables.cpp", "w") as file:
        file.write(
            title_line("Tables for iconv's East Asian sets", "---===//")
            + "\n"
            + LICENSE
            + notice
            + "\n"
            + '#include "src/iconv/cjk_tables.h"\n'
            + "\n"
            + "namespace LIBC_NAMESPACE_DECL {\n"
            + "namespace iconv_internal {\n"
            + "\n"
            + "// clang-format off\n"
            + "\n\n".join(definitions)
            + "\n// clang-format on\n"
            + "\n"
            + "} // namespace iconv_internal\n"
            + "} // namespace LIBC_NAMESPACE_DECL\n"
        )


def main() -> None:
    if len(argv) != 3:
        exit(f"usage: {argv[0]} <llvm-project> <mapping folder>")
    root, mappings = argv[1], argv[2]
    tables = []

    def apply_rules(file_name, rules):
        mapping = read_mapping(f"{mappings}/{file_name}")
        if rules.get("rows"):
            mapping = {
                byte: mapping.get((byte << 8) | byte) if value == 0 and byte else value
                for byte, value in mapping.items()
                if byte < 0x100
            }
        if rules.get("controls"):
            for byte in list(range(0x20)) + [0x7F]:
                mapping.setdefault(byte, byte)
        # A set built on another: the base's high bytes from |below| up, then
        # the set's own mapping over them.
        if "overlay" in rules:
            base = {b: v for b, v in mapping.items() if b < 0x80 or b >= 0xC0}
            overlay = read_mapping(f"{mappings}/{rules['overlay'][0]}")
            base.update({b: v for b, v in overlay.items() if b >= rules["below"]})
            mapping = base
        mapping.update(rules.get("changes", {}))
        for byte in rules.get("unassigned", []):
            mapping.pop(byte, None)
        return mapping

    mappings_by_name = {}
    for name, file_name, _, *rules in SINGLE_BYTE:
        mapping = apply_rules(file_name, rules[0] if rules else {})
        mappings_by_name[name] = mapping
        sequences = split_sequences(mapping)
        tables.append(format_table(name, high_half(name, mapping)))
        if sequences:
            tables.append(format_sequences(name, sequences))
    for name, file_name, _, *rules in FULL:
        mapping = apply_rules(file_name, rules[0] if rules else {})
        mappings_by_name[name] = mapping
        tables.append(
            format_table(name, [mapping.get(byte, UNASSIGNED) for byte in range(256)])
        )
    classes, decompositions = read_unicode_data(f"{mappings}/UnicodeData.txt")
    for name in COMBINING:
        tables.append(
            format_combining(
                name,
                *combining_tables(
                    name, mappings_by_name[name], classes, decompositions
                ),
            )
        )

    definitions, declarations = japanese_tables(mappings)
    chinese = chinese_tables(mappings)
    definitions += chinese[0]
    declarations += chinese[1]
    traditional = traditional_chinese_tables(mappings)
    definitions += traditional[0]
    declarations += traditional[1]
    korean = korean_tables(mappings)
    definitions += korean[0]
    declarations += korean[1]
    write_cjk_tables(root, definitions, declarations)

    title = "//===-- Tables for iconv's single byte sets "
    title += "-" * (80 - len(title) - len("*- C++ -*-===//")) + "*- C++ -*-===//"
    with open(f"{root}/libc/src/iconv/single_byte_tables.h", "w") as file:
        file.write(
            title
            + "\n"
            + "//\n"
            + "// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.\n"
            + "// See https://llvm.org/LICENSE.txt for license information.\n"
            + "// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception\n"
            + "//\n"
            + "//===----------------------------------------------------------------------===//\n"
            + "//\n"
            + "// DO NOT EDIT MANUALLY. This file is generated by\n"
            + "// libc/utils/iconv_utils/gen.py from the mapping files it lists.\n"
            + "//\n"
            + "// A _HIGH table gives the code points of bytes 0x80 to 0xFF, for a set which\n"
            + "// agrees with ASCII below them; a _FULL table gives all 256. 0xFFFD marks a\n"
            + "// byte the set does not assign, and 0xFFFF one which stands for the several\n"
            + "// characters its _SEQUENCE_LIST gives.\n"
            + "//\n"
            + "// For a set which joins a character and the combining mark after it, a\n"
            + "// _COMPOSITIONS table gives the pairs it joins and what they make, in the\n"
            + "// order of the pair, and a _DECOMPOSITIONS table gives the bytes it writes a\n"
            + "// character it has no byte for as, in the order of the character.\n"
            + "//\n"
            + "//===----------------------------------------------------------------------===//\n"
            + "\n"
            + "#ifndef LLVM_LIBC_SRC_ICONV_SINGLE_BYTE_TABLES_H\n"
            + "#define LLVM_LIBC_SRC_ICONV_SINGLE_BYTE_TABLES_H\n"
            + "\n"
            + '#include "hdr/stdint_proxy.h"\n'
            + '#include "src/__support/macros/config.h"\n'
            + '#include "src/iconv/combining.h"\n'
            + '#include "src/iconv/sequences.h"\n'
            + "\n"
            + "namespace LIBC_NAMESPACE_DECL {\n"
            + "namespace iconv_internal {\n"
            + "\n"
            + "// clang-format off\n"
        )
        file.write("\n\n".join(tables))
        file.write(
            "\n// clang-format on\n"
            + "\n"
            + "} // namespace iconv_internal\n"
            + "} // namespace LIBC_NAMESPACE_DECL\n"
            + "\n"
            + "#endif // LLVM_LIBC_SRC_ICONV_SINGLE_BYTE_TABLES_H\n"
        )


if __name__ == "__main__":
    main()
