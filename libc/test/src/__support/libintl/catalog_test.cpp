//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the tests of the message catalog reader.
///
//===----------------------------------------------------------------------===//

#include "src/__support/libintl/catalog.h"
#include "test/UnitTest/Test.h"

using LIBC_NAMESPACE::libintl::MessageCatalog;

namespace {

struct Message {
  const char *original;
  unsigned original_size;
  const char *translation;
  unsigned translation_size;
};

#define MSG(o, t) {o, sizeof(o) - 1, t, sizeof(t) - 1}

constexpr char HEADER[] = "Content-Type: text/plain; charset=UTF-8\n"
                          "Plural-Forms: nplurals=2; plural=(n != 1);\n";

// In the order of the originals, which is how msgfmt writes them.
const Message MESSAGES[] = {
    {"", 0, HEADER, sizeof(HEADER) - 1},
    MSG("%d file\0%d files", "%d Datei\0%d Dateien"),
    MSG("Hello", "Hallo"),
    MSG("Zebra", "Zebra!"),
};
constexpr unsigned COUNT = 4;

struct Writer {
  char bytes[1024];
  bool big;
  unsigned put(unsigned at, unsigned v) {
    for (int i = 0; i < 4; ++i)
      bytes[at + i] = static_cast<char>(big ? v >> (24 - 8 * i) : v >> (8 * i));
    return at + 4;
  }
};

unsigned build(Writer &w, unsigned revision) {
  const unsigned originals = 28;
  const unsigned translations = originals + COUNT * 8;
  unsigned at = translations + COUNT * 8;
  w.put(0, 0x950412de);
  w.put(4, revision);
  w.put(8, COUNT);
  w.put(12, originals);
  w.put(16, translations);
  w.put(20, 0);
  w.put(24, 0);
  for (unsigned i = 0; i < COUNT; ++i) {
    w.put(originals + i * 8, MESSAGES[i].original_size);
    w.put(originals + i * 8 + 4, at);
    for (unsigned j = 0; j < MESSAGES[i].original_size; ++j)
      w.bytes[at++] = MESSAGES[i].original[j];
    w.bytes[at++] = '\0';
  }
  for (unsigned i = 0; i < COUNT; ++i) {
    w.put(translations + i * 8, MESSAGES[i].translation_size);
    w.put(translations + i * 8 + 4, at);
    for (unsigned j = 0; j < MESSAGES[i].translation_size; ++j)
      w.bytes[at++] = MESSAGES[i].translation[j];
    w.bytes[at++] = '\0';
  }
  return at;
}

} // namespace

TEST(LlvmLibcMessageCatalogTest, FindsMessages) {
  Writer w{};
  unsigned size = build(w, 0);
  MessageCatalog c;
  ASSERT_TRUE(c.load(w.bytes, size));
  ASSERT_STREQ(c.translate("Hello", false, 0), "Hallo");
  ASSERT_STREQ(c.translate("Zebra", false, 0), "Zebra!");
  ASSERT_TRUE(c.translate("Absent", false, 0) == nullptr);
  ASSERT_TRUE(c.translate("A", false, 0) == nullptr);
  ASSERT_TRUE(c.translate("Zzz", false, 0) == nullptr);
  ASSERT_TRUE(c.header().starts_with("Content-Type"));
}

TEST(LlvmLibcMessageCatalogTest, ChoosesPluralForms) {
  Writer w{};
  unsigned size = build(w, 0);
  MessageCatalog c;
  ASSERT_TRUE(c.load(w.bytes, size));
  ASSERT_STREQ(c.translate("%d file", true, 1), "%d Datei");
  ASSERT_STREQ(c.translate("%d file", true, 2), "%d Dateien");
  // Asked without a count, the first form is the answer.
  ASSERT_STREQ(c.translate("%d file", false, 0), "%d Datei");
  // A message without forms has one whatever the count is.
  ASSERT_TRUE(c.translate("Hello", true, 5) == nullptr);
}

TEST(LlvmLibcMessageCatalogTest, ReadsEitherByteOrder) {
  Writer w{};
  w.big = true;
  unsigned size = build(w, 0);
  MessageCatalog c;
  ASSERT_TRUE(c.load(w.bytes, size));
  ASSERT_STREQ(c.translate("Hello", false, 0), "Hallo");
}

TEST(LlvmLibcMessageCatalogTest, RevisionOneIsReadAndTwoIsNot) {
  Writer w{};
  unsigned size = build(w, 1 << 16 | 1);
  MessageCatalog one;
  ASSERT_TRUE(one.load(w.bytes, size));
  ASSERT_STREQ(one.translate("Hello", false, 0), "Hallo");
  size = build(w, 2 << 16);
  MessageCatalog two;
  ASSERT_FALSE(two.load(w.bytes, size));
}

TEST(LlvmLibcMessageCatalogTest, RefusesDamagedFiles) {
  Writer w{};
  unsigned size = build(w, 0);
  MessageCatalog c;
  ASSERT_FALSE(c.load(w.bytes, 10));
  // Cut short, the tables or the strings they point at lie past the end.
  ASSERT_FALSE(c.load(w.bytes, 60));
  MessageCatalog d;
  w.bytes[0] = 'x';
  ASSERT_FALSE(d.load(w.bytes, size));
  // A string which does not end in a null where its length says.
  Writer v{};
  size = build(v, 0);
  v.bytes[size - 1] = 'x';
  MessageCatalog e;
  e.load(v.bytes, size);
  ASSERT_TRUE(e.translate("Zebra", false, 0) == nullptr);
}
