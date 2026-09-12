#!/usr/bin/env python3
#
# ===- Generate the iconv character set tables ---------------*- python -*--==#
#
# Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# ==------------------------------------------------------------------------==#

from sys import argv, exit

UNICODE = "https://www.unicode.org/Public/MAPPINGS"
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
]

# The value a table holds for a byte the set does not assign.
UNASSIGNED = 0xFFFD


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


def read_mapping(path: str) -> dict[int, int]:
    if path.endswith(".646"):
        return read_iso646_mapping(path)
    if path.endswith(".src"):
        return read_citrus_mapping(path)
    if path.endswith(".ucm"):
        return read_ucm_mapping(path)
    return read_unicode_mapping(path)


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


def main() -> None:
    if len(argv) != 3:
        exit(f"usage: {argv[0]} <llvm-project> <mapping folder>")
    root, mappings = argv[1], argv[2]
    tables = []

    def apply_rules(file_name, rules):
        mapping = read_mapping(f"{mappings}/{file_name}")
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

    for name, file_name, _, *rules in SINGLE_BYTE:
        mapping = apply_rules(file_name, rules[0] if rules else {})
        tables.append(format_table(name, high_half(name, mapping)))
    for name, file_name, _, *rules in FULL:
        mapping = apply_rules(file_name, rules[0] if rules else {})
        tables.append(
            format_table(name, [mapping.get(byte, UNASSIGNED) for byte in range(256)])
        )

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
            + "// byte the set does not assign.\n"
            + "//\n"
            + "//===----------------------------------------------------------------------===//\n"
            + "\n"
            + "#ifndef LLVM_LIBC_SRC_ICONV_SINGLE_BYTE_TABLES_H\n"
            + "#define LLVM_LIBC_SRC_ICONV_SINGLE_BYTE_TABLES_H\n"
            + "\n"
            + '#include "hdr/stdint_proxy.h"\n'
            + '#include "src/__support/macros/config.h"\n'
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
