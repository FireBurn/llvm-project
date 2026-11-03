//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the tests of translation through message catalogs.
///
//===----------------------------------------------------------------------===//

#include "hdr/locale_macros.h"
#include "src/libintl/bind_textdomain_codeset.h"
#include "src/libintl/bindtextdomain.h"
#include "src/libintl/dgettext.h"
#include "src/libintl/dngettext.h"
#include "src/libintl/textdomain.h"
#include "src/locale/setlocale.h"
#include "src/stdio/fclose.h"
#include "src/stdio/fopen.h"
#include "src/stdio/fwrite.h"
#include "src/stdio/remove.h"
#include "src/stdlib/setenv.h"
#include "src/stdlib/unsetenv.h"
#include "src/sys/stat/mkdir.h"
#include "src/unistd/rmdir.h"
#include "test/UnitTest/Test.h"

namespace {

struct Message {
  const char *original;
  unsigned original_size;
  const char *translation;
  unsigned translation_size;
};

// Messages carry their sizes since plural forms are joined by nulls.
#define MSG(o, t) {o, sizeof(o) - 1, t, sizeof(t) - 1}

void put32(char *at, unsigned v) {
  at[0] = static_cast<char>(v);
  at[1] = static_cast<char>(v >> 8);
  at[2] = static_cast<char>(v >> 16);
  at[3] = static_cast<char>(v >> 24);
}

// Writes a catalog in the .mo format, little endian, from messages already
// ordered by their originals.
unsigned build(char *out, const Message *messages, unsigned count) {
  const unsigned originals = 28;
  const unsigned translations = originals + count * 8;
  unsigned at = translations + count * 8;
  put32(out, 0x950412de);
  put32(out + 4, 0);
  put32(out + 8, count);
  put32(out + 12, originals);
  put32(out + 16, translations);
  put32(out + 20, 0);
  put32(out + 24, 0);
  for (unsigned i = 0; i < count; ++i) {
    put32(out + originals + i * 8, messages[i].original_size);
    put32(out + originals + i * 8 + 4, at);
    for (unsigned j = 0; j < messages[i].original_size; ++j)
      out[at++] = messages[i].original[j];
    out[at++] = '\0';
  }
  for (unsigned i = 0; i < count; ++i) {
    put32(out + translations + i * 8, messages[i].translation_size);
    put32(out + translations + i * 8 + 4, at);
    for (unsigned j = 0; j < messages[i].translation_size; ++j)
      out[at++] = messages[i].translation[j];
    out[at++] = '\0';
  }
  return at;
}

// A path under the test directory, built from parts.
struct Path {
  char text[4096];
  unsigned size = 0;
  Path(const char *a, const char *b = "", const char *c = "",
       const char *d = "", const char *e = "") {
    const char *parts[] = {a, b, c, d, e};
    for (const char *part : parts)
      for (; *part != '\0'; ++part)
        text[size++] = *part;
    text[size] = '\0';
  }
};

// Copied out of the path the framework returns, which frees its characters
// with it. Bazel's output directory is long, so there is room for it.
char ROOT[4096];
const char *root() {
  if (ROOT[0] == '\0') {
    const auto path = libc_make_test_file_path("libintl_catalogs");
    const char *text = path;
    for (unsigned i = 0; (ROOT[i] = text[i]) != '\0'; ++i)
      ;
  }
  return ROOT;
}

void make_catalog(const char *locale, const char *domain,
                  const Message *messages, unsigned count) {
  LIBC_NAMESPACE::mkdir(Path(root()).text, 0755);
  LIBC_NAMESPACE::mkdir(Path(root(), "/", locale).text, 0755);
  LIBC_NAMESPACE::mkdir(Path(root(), "/", locale, "/LC_MESSAGES").text, 0755);
  static char bytes[4096];
  unsigned size = build(bytes, messages, count);
  ::FILE *f = LIBC_NAMESPACE::fopen(
      Path(root(), "/", locale, "/LC_MESSAGES/", domain).text, "wb");
  ASSERT_TRUE(f != nullptr);
  ASSERT_EQ(LIBC_NAMESPACE::fwrite(bytes, 1, size, f),
            static_cast<size_t>(size));
  LIBC_NAMESPACE::fclose(f);
}

void make_aliases() {
  ::FILE *f = LIBC_NAMESPACE::fopen(Path(root(), "/locale.alias").text, "w");
  ASSERT_TRUE(f != nullptr);
  const char text[] = "# aliases\nrussian ru_RU.UTF-8\nGERMAN\tgerman_alias\n"
                      "german_alias de_DE.ISO-8859-1\n";
  LIBC_NAMESPACE::fwrite(text, 1, sizeof(text) - 1, f);
  LIBC_NAMESPACE::fclose(f);
}

void remove_catalog(const char *locale, const char *domain) {
  LIBC_NAMESPACE::remove(Path(root(), "/", locale, "/LC_MESSAGES/", domain).text);
  LIBC_NAMESPACE::rmdir(Path(root(), "/", locale, "/LC_MESSAGES").text);
  LIBC_NAMESPACE::rmdir(Path(root(), "/", locale).text);
}

constexpr char RUSSIAN_HEADER[] =
    "Content-Type: text/plain; charset=UTF-8\n"
    "Plural-Forms: nplurals=3; plural=n%10==1 && n%100!=11 ? 0 : "
    "n%10>=2 && n%10<=4 && (n%100<10 || n%100>=20) ? 1 : 2;\n";

// The originals are in byte order: the empty header, then '%' before 'H'.
const Message RUSSIAN[] = {
    {"", 0, RUSSIAN_HEADER, sizeof(RUSSIAN_HEADER) - 1},
    MSG("%d file\0%d files",
        "%d \xd1\x84\xd0\xb0\xd0\xb9\xd0\xbb\0"
        "%d \xd1\x84\xd0\xb0\xd0\xb9\xd0\xbb\xd0\xb0\0"
        "%d \xd1\x84\xd0\xb0\xd0\xb9\xd0\xbb\xd0\xbe\xd0\xb2"),
    MSG("Hello", "\xd0\x9f\xd1\x80\xd0\xb8\xd0\xb2\xd0\xb5\xd1\x82"),
    // A message with a context, which the macros of gettext.h join with 004.
    MSG("menu\004Open",
        "\xd0\x9e\xd1\x82\xd0\xba\xd1\x80\xd1\x8b\xd1\x82\xd1\x8c"),
};

// A catalog in ISO-8859-1: "Gruess Gott" with the u umlaut and the sharp s.
constexpr char LATIN1_HEADER[] =
    "Content-Type: text/plain; charset=ISO-8859-1\n";
const Message GERMAN[] = {
    {"", 0, LATIN1_HEADER, sizeof(LATIN1_HEADER) - 1},
    MSG("Greeting", "Gr\xfc\xdf Gott"),
};

class LlvmLibcLibintlCatalogTest : public LIBC_NAMESPACE::testing::Test {
public:
  void SetUp() override {
    LIBC_NAMESPACE::unsetenv("LANGUAGE");
    make_catalog("ru", "app.mo", RUSSIAN, 4);
    make_catalog("de_DE", "app.mo", GERMAN, 2);
    make_aliases();
    ASSERT_TRUE(LIBC_NAMESPACE::bindtextdomain("app", root()) != nullptr);
  }
  void TearDown() override {
    LIBC_NAMESPACE::setlocale(LC_ALL, "C");
    LIBC_NAMESPACE::unsetenv("LANGUAGE");
    remove_catalog("ru", "app.mo");
    remove_catalog("de_DE", "app.mo");
    LIBC_NAMESPACE::remove(Path(root(), "/locale.alias").text);
    LIBC_NAMESPACE::rmdir(Path(root()).text);
  }
};

} // namespace

TEST_F(LlvmLibcLibintlCatalogTest, CLocaleIsNotTranslated) {
  LIBC_NAMESPACE::setlocale(LC_ALL, "C");
  ASSERT_STREQ(LIBC_NAMESPACE::dgettext("app", "Hello"), "Hello");
}

TEST_F(LlvmLibcLibintlCatalogTest, Translates) {
  LIBC_NAMESPACE::setlocale(LC_ALL, "ru_RU.UTF-8");
  ASSERT_STREQ(LIBC_NAMESPACE::dgettext("app", "Hello"),
               "\xd0\x9f\xd1\x80\xd0\xb8\xd0\xb2\xd0\xb5\xd1\x82");
  // A message the catalog does not have comes back as it is.
  ASSERT_STREQ(LIBC_NAMESPACE::dgettext("app", "Goodbye"), "Goodbye");
  // So does one in a domain with no catalog.
  ASSERT_STREQ(LIBC_NAMESPACE::dgettext("other", "Hello"), "Hello");
  // The empty message is the header.
  ASSERT_TRUE(LIBC_NAMESPACE::dgettext("app", "")[0] == 'C');
}

TEST_F(LlvmLibcLibintlCatalogTest, TextdomainSetsTheDefault) {
  LIBC_NAMESPACE::setlocale(LC_ALL, "ru_RU.UTF-8");
  LIBC_NAMESPACE::textdomain("app");
  ASSERT_STREQ(LIBC_NAMESPACE::dgettext(nullptr, "Hello"),
               "\xd0\x9f\xd1\x80\xd0\xb8\xd0\xb2\xd0\xb5\xd1\x82");
  LIBC_NAMESPACE::textdomain("messages");
}

TEST_F(LlvmLibcLibintlCatalogTest, PluralFormsFollowTheCatalog) {
  LIBC_NAMESPACE::setlocale(LC_ALL, "ru_RU.UTF-8");
  auto form = [](unsigned long n) {
    return LIBC_NAMESPACE::dngettext("app", "%d file", "%d files", n);
  };
  ASSERT_STREQ(form(1), "%d \xd1\x84\xd0\xb0\xd0\xb9\xd0\xbb");
  ASSERT_STREQ(form(21), "%d \xd1\x84\xd0\xb0\xd0\xb9\xd0\xbb");
  ASSERT_STREQ(form(2), "%d \xd1\x84\xd0\xb0\xd0\xb9\xd0\xbb\xd0\xb0");
  ASSERT_STREQ(form(24), "%d \xd1\x84\xd0\xb0\xd0\xb9\xd0\xbb\xd0\xb0");
  ASSERT_STREQ(form(5), "%d \xd1\x84\xd0\xb0\xd0\xb9\xd0\xbb\xd0\xbe\xd0\xb2");
  ASSERT_STREQ(form(11), "%d \xd1\x84\xd0\xb0\xd0\xb9\xd0\xbb\xd0\xbe\xd0\xb2");
}

TEST_F(LlvmLibcLibintlCatalogTest, CountsWithoutACatalogUseEnglish) {
  LIBC_NAMESPACE::setlocale(LC_ALL, "ru_RU.UTF-8");
  ASSERT_STREQ(LIBC_NAMESPACE::dngettext("other", "a file", "files", 1),
               "a file");
  ASSERT_STREQ(LIBC_NAMESPACE::dngettext("other", "a file", "files", 2),
               "files");
}

TEST_F(LlvmLibcLibintlCatalogTest, LanguageOverridesTheLocale) {
  LIBC_NAMESPACE::setlocale(LC_ALL, "de_DE.UTF-8");
  // A language without a catalog is passed over for the next one.
  LIBC_NAMESPACE::setenv("LANGUAGE", "xx:ru", 1);
  ASSERT_STREQ(LIBC_NAMESPACE::dgettext("app", "Hello"),
               "\xd0\x9f\xd1\x80\xd0\xb8\xd0\xb2\xd0\xb5\xd1\x82");
  // C ends the list.
  LIBC_NAMESPACE::setenv("LANGUAGE", "C:ru", 1);
  ASSERT_STREQ(LIBC_NAMESPACE::dgettext("app", "Hello"), "Hello");
}

TEST_F(LlvmLibcLibintlCatalogTest, ConvertsToTheLocalesCharacterSet) {
  LIBC_NAMESPACE::setlocale(LC_ALL, "de_DE.UTF-8");
  // Latin-1 in the catalog, UTF-8 asked for.
  ASSERT_STREQ(LIBC_NAMESPACE::dgettext("app", "Greeting"),
               "Gr\xc3\xbc\xc3\x9f Gott");
}

TEST_F(LlvmLibcLibintlCatalogTest, ConvertsToTheBoundCodeset) {
  LIBC_NAMESPACE::setlocale(LC_ALL, "ru_RU.UTF-8");
  ASSERT_STREQ(LIBC_NAMESPACE::bind_textdomain_codeset("app", "KOI8-R"),
               "KOI8-R");
  // "Hello" in KOI8-R.
  ASSERT_STREQ(LIBC_NAMESPACE::dgettext("app", "Hello"),
               "\xf0\xd2\xc9\xd7\xc5\xd4");
  LIBC_NAMESPACE::bind_textdomain_codeset("app", "UTF-8");
}

TEST_F(LlvmLibcLibintlCatalogTest, UnrepresentableCharactersAreTransliterated) {
  LIBC_NAMESPACE::setlocale(LC_ALL, "de_DE.UTF-8");
  LIBC_NAMESPACE::bind_textdomain_codeset("app", "US-ASCII");
  // The sharp s has a transliteration and the u umlaut has none that fits.
  const char *text = LIBC_NAMESPACE::dgettext("app", "Greeting");
  ASSERT_TRUE(text[0] == 'G' && text[1] == 'r');
  for (const char *p = text; *p != '\0'; ++p)
    ASSERT_EQ(static_cast<int>(*p) & 0x80, 0);
  LIBC_NAMESPACE::bind_textdomain_codeset("app", "UTF-8");
}

TEST_F(LlvmLibcLibintlCatalogTest, ABadCatalogIsIgnored) {
  make_catalog("fr", "app.mo", RUSSIAN, 4);
  // Cut it short so the tables run past the end of the file.
  ::FILE *f =
      LIBC_NAMESPACE::fopen(Path(root(), "/fr/LC_MESSAGES/app.mo").text, "wb");
  ASSERT_TRUE(f != nullptr);
  LIBC_NAMESPACE::fwrite("\xde\x12\x04\x95\0\0\0\0\xff\xff\xff\x7f", 1, 12, f);
  LIBC_NAMESPACE::fclose(f);
  LIBC_NAMESPACE::setlocale(LC_ALL, "fr_FR.UTF-8");
  ASSERT_STREQ(LIBC_NAMESPACE::dgettext("app", "Hello"), "Hello");
  remove_catalog("fr", "app.mo");
}

TEST_F(LlvmLibcLibintlCatalogTest, MessagesWithAContext) {
  LIBC_NAMESPACE::setlocale(LC_ALL, "ru_RU.UTF-8");
  ASSERT_STREQ(LIBC_NAMESPACE::dgettext("app", "menu\004Open"),
               "\xd0\x9e\xd1\x82\xd0\xba\xd1\x80\xd1\x8b\xd1\x82\xd1\x8c");
  // The same words under another context are not found.
  ASSERT_STREQ(LIBC_NAMESPACE::dgettext("app", "door\004Open"), "door\004Open");
}

TEST_F(LlvmLibcLibintlCatalogTest, AliasesStandForLocales) {
  LIBC_NAMESPACE::setlocale(LC_ALL, "de_DE.UTF-8");
  // "russian" is no directory but an alias of a locale which is one.
  LIBC_NAMESPACE::setenv("LANGUAGE", "russian", 1);
  ASSERT_STREQ(LIBC_NAMESPACE::dgettext("app", "Hello"),
               "\xd0\x9f\xd1\x80\xd0\xb8\xd0\xb2\xd0\xb5\xd1\x82");
  // An alias of an alias, matched without regard to case.
  LIBC_NAMESPACE::setenv("LANGUAGE", "german", 1);
  ASSERT_STREQ(LIBC_NAMESPACE::dgettext("app", "Greeting"),
               "Gr\xc3\xbc\xc3\x9f Gott");
}
