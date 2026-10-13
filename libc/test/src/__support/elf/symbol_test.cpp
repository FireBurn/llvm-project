//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for GNU hash symbol lookup
///
//===----------------------------------------------------------------------===//

#include "src/__support/elf/symbol.h"
#include "test/UnitTest/Test.h"

using LIBC_NAMESPACE::elf::GnuSymbolTable;

namespace {

// A DT_GNU_HASH table laid out the way a linker emits one: header, Bloom
// filter, buckets, then the hash array. Index 0 of .dynsym is the reserved
// null entry, so symoffset is 1 and the hashed symbols are sorted by bucket.
//
// Symbols, in bucket order: strlen, printf, malloc, free, fopen.
struct Table {
  uint32_t nbuckets = 3;
  uint32_t symoffset = 1;
  uint32_t bloom_size = 2;
  uint32_t bloom_shift = 6;
  // Every bit set, so that nothing is turned away before the buckets are
  // read. The filter is as wide as an address, which differs between the ELF
  // classes, and a real linker's bits for one would be wrong for the other.
  ElfW(Addr) bloom[2] = { ~ElfW(Addr)(0), ~ElfW(Addr)(0) };
  uint32_t buckets[3] = {1, 2, 0};
  uint32_t chain[5] = {0x1c93bb9d, 0x156b2bb8, 0x0d39ad3c, 0x7c96f086,
                       0x0f738b7d};
};

const Table TABLE;

// Offsets into STRTAB below.
const char STRTAB[] = "\0strlen\0printf\0malloc\0free\0fopen";

ElfW(Sym) make_sym(uint32_t name_offset, ElfW(Addr) value) {
  ElfW(Sym) s{};
  s.st_name = name_offset;
  s.st_value = value;
  s.st_info = ELF64_ST_INFO(STB_GLOBAL, STT_FUNC);
  s.st_shndx = 1;
  return s;
}

const ElfW(Sym) SYMTAB[] = {
    make_sym(0, 0),       // reserved null entry
    make_sym(1, 0x1000),  // strlen
    make_sym(8, 0x2000),  // printf
    make_sym(15, 0x3000), // malloc
    make_sym(22, 0x4000), // free
    make_sym(27, 0x5000), // fopen
};

GnuSymbolTable table() { return GnuSymbolTable(&TABLE, SYMTAB, STRTAB); }

} // anonymous namespace

TEST(LlvmLibcElfSymbolTest, DefaultIsEmpty) {
  GnuSymbolTable empty;
  EXPECT_TRUE(empty.empty());
  EXPECT_EQ(empty.lookup("printf"), static_cast<const ElfW(Sym) *>(nullptr));
}

TEST(LlvmLibcElfSymbolTest, FindsEverySymbol) {
  GnuSymbolTable t = table();
  ASSERT_FALSE(t.empty());
  // Each name must resolve to its own entry, including the ones that share a
  // bucket and are only separated by walking the chain.
  ASSERT_TRUE(t.lookup("strlen") != nullptr);
  EXPECT_EQ(t.lookup("strlen")->st_value, ElfW(Addr)(0x1000));
  ASSERT_TRUE(t.lookup("printf") != nullptr);
  EXPECT_EQ(t.lookup("printf")->st_value, ElfW(Addr)(0x2000));
  ASSERT_TRUE(t.lookup("malloc") != nullptr);
  EXPECT_EQ(t.lookup("malloc")->st_value, ElfW(Addr)(0x3000));
  ASSERT_TRUE(t.lookup("free") != nullptr);
  EXPECT_EQ(t.lookup("free")->st_value, ElfW(Addr)(0x4000));
  ASSERT_TRUE(t.lookup("fopen") != nullptr);
  EXPECT_EQ(t.lookup("fopen")->st_value, ElfW(Addr)(0x5000));
}

TEST(LlvmLibcElfSymbolTest, MissingNamesReturnNull) {
  GnuSymbolTable t = table();
  EXPECT_EQ(t.lookup("calloc"), static_cast<const ElfW(Sym) *>(nullptr));
  EXPECT_EQ(t.lookup(""), static_cast<const ElfW(Sym) *>(nullptr));
  EXPECT_EQ(t.lookup("qsort"), static_cast<const ElfW(Sym) *>(nullptr));
}

TEST(LlvmLibcElfSymbolTest, PrefixesDoNotMatch) {
  // A chain walk that compared only the hash, or compared with a prefix match,
  // would wrongly resolve these.
  GnuSymbolTable t = table();
  EXPECT_EQ(t.lookup("printf_"), static_cast<const ElfW(Sym) *>(nullptr));
  EXPECT_EQ(t.lookup("print"), static_cast<const ElfW(Sym) *>(nullptr));
  EXPECT_EQ(t.lookup("fre"), static_cast<const ElfW(Sym) *>(nullptr));
}

namespace {

// Two definitions of one name, the older one first and marked hidden, as a
// library which keeps a compatibility version emits them.
struct VersionedTable {
  uint32_t nbuckets = 1;
  uint32_t symoffset = 1;
  uint32_t bloom_size = 1;
  uint32_t bloom_shift = 6;
  ElfW(Addr) bloom[1] = { ~ElfW(Addr)(0) };
  uint32_t buckets[1] = {1};
  uint32_t chain[2] = {0, 0};
};

const char VERSTRTAB[] = "\0snd_rate";

const ElfW(Sym) VERSYMTAB[] = {
    make_sym(0, 0),
    make_sym(1, 0x1000), // snd_rate, the compatibility version
    make_sym(1, 0x2000), // snd_rate, the default version
};

} // anonymous namespace

TEST(LlvmLibcElfSymbolTest, HiddenVersionYieldsToDefault) {
  VersionedTable t;
  const uint32_t hash = LIBC_NAMESPACE::elf::gnu_hash("snd_rate");
  t.chain[0] = hash & ~1u;
  t.chain[1] = hash | 1u;
  const uint16_t versym[] = {0, 0x8002, 0x0003};
  GnuSymbolTable table(&t, VERSYMTAB, VERSTRTAB, versym);
  const ElfW(Sym) *sym = table.lookup("snd_rate");
  ASSERT_TRUE(sym != nullptr);
  EXPECT_EQ(sym->st_value, ElfW(Addr)(0x2000));
}

TEST(LlvmLibcElfSymbolTest, HiddenVersionIsUsedWhenAlone) {
  VersionedTable t;
  const uint32_t hash = LIBC_NAMESPACE::elf::gnu_hash("snd_rate");
  t.chain[0] = hash & ~1u;
  t.chain[1] = hash | 1u;
  const uint16_t versym[] = {0, 0x8002, 0x8003};
  GnuSymbolTable table(&t, VERSYMTAB, VERSTRTAB, versym);
  const ElfW(Sym) *sym = table.lookup("snd_rate");
  ASSERT_TRUE(sym != nullptr);
  EXPECT_EQ(sym->st_value, ElfW(Addr)(0x1000));
}

namespace {

// A library with a verdef section naming version 2 "OLD_1" and version 3
// "NEW_2", laid out the way a linker emits one: each definition followed by
// its single auxiliary entry.
struct VersionDefs {
  ElfW(Verdef) old_def;
  ElfW(Verdaux) old_aux;
  ElfW(Verdef) new_def;
  ElfW(Verdaux) new_aux;
};

const char DEFSTRTAB[] = "\0snd_rate\0OLD_1\0NEW_2";

VersionDefs make_defs() {
  VersionDefs d{};
  d.old_def.vd_version = 1;
  d.old_def.vd_cnt = 1;
  d.old_def.vd_ndx = 2;
  d.old_def.vd_aux = sizeof(ElfW(Verdef));
  d.old_def.vd_next = sizeof(ElfW(Verdef)) + sizeof(ElfW(Verdaux));
  d.old_aux.vda_name = 10;
  d.new_def.vd_version = 1;
  d.new_def.vd_cnt = 1;
  d.new_def.vd_ndx = 3;
  d.new_def.vd_aux = sizeof(ElfW(Verdef));
  d.new_aux.vda_name = 16;
  return d;
}

} // anonymous namespace

TEST(LlvmLibcElfSymbolTest, RequestedVersionIsHonoured) {
  VersionedTable t;
  const uint32_t hash = LIBC_NAMESPACE::elf::gnu_hash("snd_rate");
  t.chain[0] = hash & ~1u;
  t.chain[1] = hash | 1u;
  const uint16_t versym[] = {0, 0x8002, 0x0003};
  const VersionDefs defs = make_defs();
  GnuSymbolTable table(&t, VERSYMTAB, DEFSTRTAB, versym, &defs.old_def);
  const ElfW(Sym) *old_sym = table.lookup("snd_rate", "OLD_1");
  ASSERT_TRUE(old_sym != nullptr);
  EXPECT_EQ(old_sym->st_value, ElfW(Addr)(0x1000));
  const ElfW(Sym) *new_sym = table.lookup("snd_rate", "NEW_2");
  ASSERT_TRUE(new_sym != nullptr);
  EXPECT_EQ(new_sym->st_value, ElfW(Addr)(0x2000));
  // A version the library does not have falls back to its default.
  const ElfW(Sym) *other = table.lookup("snd_rate", "OTHER_9");
  ASSERT_TRUE(other != nullptr);
  EXPECT_EQ(other->st_value, ElfW(Addr)(0x2000));
}
