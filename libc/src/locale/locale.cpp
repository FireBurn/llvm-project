//===-- The locale in force -----------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/locale/locale.h"

#include "hdr/locale_macros.h"
#include "src/__support/CPP/string_view.h"
#include "src/__support/common.h"
#include "src/__support/ctype_utils.h"
#include "src/__support/macros/config.h"
#include "src/stdlib/mb_cur_max.h"
#include "src/unistd/environ.h"

namespace LIBC_NAMESPACE_DECL {

__locale_t c_locale = {nullptr};

locale_t locale = nullptr;

namespace {

// Each category keeps its own name, which is what callers are told when they
// ask, and what a composite LC_ALL string is built from.
__locale_data current[NUM_LOCALE_CATEGORIES] = {
    {{'C', '\0'}, false}, {{'C', '\0'}, false}, {{'C', '\0'}, false},
    {{'C', '\0'}, false}, {{'C', '\0'}, false}, {{'C', '\0'}, false}};

const char *const CATEGORY_NAMES[NUM_LOCALE_CATEGORIES] = {
    "LC_CTYPE",   "LC_NUMERIC",  "LC_TIME",
    "LC_COLLATE", "LC_MONETARY", "LC_MESSAGES"};

// "LC_CTYPE=a;LC_NUMERIC=b;..." at its longest.
char composite[NUM_LOCALE_CATEGORIES * (MAX_LOCALE_NAME_SIZE + 16)];

} // anonymous namespace

namespace internal {

namespace {

// The part of a name after the full stop, without any modifier that follows
// it. Empty where the name states no codeset.
cpp::string_view codeset_of(cpp::string_view name) {
  size_t stop = name.find_first_of('.');
  if (stop == cpp::string_view::npos)
    return cpp::string_view();
  cpp::string_view codeset = name.substr(stop + 1);
  size_t at = codeset.find_first_of('@');
  if (at != cpp::string_view::npos)
    codeset = codeset.substr(0, at);
  return codeset;
}

// Compares without regard to case, or to the punctuation character sets are
// written with either way.
bool same_codeset(cpp::string_view codeset, const char *expected) {
  size_t read = 0;
  for (size_t i = 0; i < codeset.size(); ++i) {
    const char ch = internal::tolower(codeset[i]);
    if (ch == '-' || ch == '_' || ch == '.')
      continue;
    if (expected[read] == '\0' || ch != expected[read])
      return false;
    ++read;
  }
  return expected[read] == '\0';
}

} // anonymous namespace

bool codeset_is_supported(cpp::string_view name) {
  cpp::string_view codeset = codeset_of(name);
  if (codeset.empty())
    return true;
  return same_codeset(codeset, "utf8") || same_codeset(codeset, "ascii") ||
         same_codeset(codeset, "usascii") || same_codeset(codeset, "646") ||
         same_codeset(codeset, "ansix341968");
}

namespace {

// What the environment says a variable is, or nullptr where it says nothing.
// The environment is read directly rather than through getenv, so a program
// that keeps its own environment and defines getenv itself, as a shell does,
// does not end up with two of them.
const char *environment_value(const char *name) {
  if (environ == nullptr)
    return nullptr;
  for (char **entry = environ; *entry != nullptr; ++entry) {
    const char *at = *entry;
    size_t i = 0;
    while (name[i] != '\0' && at[i] == name[i])
      ++i;
    if (name[i] == '\0' && at[i] == '=')
      return at + i + 1;
  }
  return nullptr;
}

} // anonymous namespace

const char *locale_from_environment(int category) {
  // LC_ALL overrides the rest where it says anything at all.
  const char *all = environment_value("LC_ALL");
  if (all != nullptr && *all != '\0')
    return all;

  if (category >= 0 && category < NUM_LOCALE_CATEGORIES) {
    const char *named = environment_value(CATEGORY_NAMES[category]);
    if (named != nullptr && *named != '\0')
      return named;
  }

  const char *lang = environment_value("LANG");
  if (lang != nullptr && *lang != '\0')
    return lang;
  return nullptr;
}

bool name_states_utf8(cpp::string_view name) {
  return same_codeset(codeset_of(name), "utf8");
}

namespace {

void store_name(int category, cpp::string_view name) {
  __locale_data &entry = current[category];
  size_t length = name.size();
  if (length >= MAX_LOCALE_NAME_SIZE)
    length = MAX_LOCALE_NAME_SIZE - 1;
  for (size_t i = 0; i < length; ++i)
    entry.name[i] = name[i];
  entry.name[length] = '\0';
  entry.utf8 = name_states_utf8(name);

  // How wide a character is follows from the codeset of LC_CTYPE, and
  // callers size their buffers by it.
  if (category == LC_CTYPE)
    set_mb_cur_max(entry.utf8 ? 4 : 1);
}

cpp::string_view name_or_environment(int category, cpp::string_view name) {
  if (!name.empty())
    return name;
  // The empty name means whatever the environment states, and the C locale
  // where it states nothing.
  const char *from_environment = locale_from_environment(category);
  return from_environment == nullptr ? cpp::string_view("C")
                                     : cpp::string_view(from_environment);
}

int category_named(cpp::string_view name) {
  for (int i = 0; i < NUM_LOCALE_CATEGORIES; ++i)
    if (name == cpp::string_view(CATEGORY_NAMES[i]))
      return i;
  return -1;
}

struct CompositePart {
  int category;
  cpp::string_view name;
};

// A composite is "LC_CTYPE=a;LC_NUMERIC=b", the form setlocale answers LC_ALL
// in when the categories differ. Every part is checked before any is taken.
bool parse_composite(cpp::string_view text, CompositePart *parts,
                     size_t &count) {
  count = 0;
  while (!text.empty()) {
    size_t stop = text.find_first_of(';');
    cpp::string_view part =
        stop == cpp::string_view::npos ? text : text.substr(0, stop);
    text = stop == cpp::string_view::npos ? cpp::string_view()
                                          : text.substr(stop + 1);
    if (part.empty())
      continue;
    size_t equals = part.find_first_of('=');
    if (equals == cpp::string_view::npos || count == NUM_LOCALE_CATEGORIES)
      return false;
    int category = category_named(part.substr(0, equals));
    cpp::string_view name = part.substr(equals + 1);
    if (category < 0 || name.empty() || !codeset_is_supported(name))
      return false;
    parts[count++] = {category, name};
  }
  return count != 0;
}

bool set_from_composite(cpp::string_view text) {
  CompositePart parts[NUM_LOCALE_CATEGORIES];
  size_t count;
  if (!parse_composite(text, parts, count))
    return false;
  for (size_t i = 0; i < count; ++i)
    store_name(parts[i].category, parts[i].name);
  return true;
}

} // anonymous namespace

const char *current_locale_name(int category) {
  if (category >= 0 && category < NUM_LOCALE_CATEGORIES)
    return current[category].name;

  // LC_ALL is one name where the categories agree. Where they differ it is
  // each category's, as glibc gives it.
  bool all_same = true;
  for (int i = 1; i < NUM_LOCALE_CATEGORIES; ++i)
    if (cpp::string_view(current[i].name) != cpp::string_view(current[0].name))
      all_same = false;
  if (all_same)
    return current[0].name;

  size_t at = 0;
  for (int i = 0; i < NUM_LOCALE_CATEGORIES; ++i) {
    if (i != 0)
      composite[at++] = ';';
    for (const char *c = CATEGORY_NAMES[i]; *c != '\0'; ++c)
      composite[at++] = *c;
    composite[at++] = '=';
    for (const char *c = current[i].name; *c != '\0'; ++c)
      composite[at++] = *c;
  }
  composite[at] = '\0';
  return composite;
}

bool locale_name_is_supported(cpp::string_view name) {
  if (name.find_first_of('=') == cpp::string_view::npos)
    return codeset_is_supported(name);
  CompositePart parts[NUM_LOCALE_CATEGORIES];
  size_t count;
  return parse_composite(name, parts, count);
}

const char *set_locale_name(int category, cpp::string_view name) {
  if (category == LC_ALL) {
    if (name.find_first_of('=') != cpp::string_view::npos)
      return set_from_composite(name) ? current_locale_name(LC_ALL) : nullptr;

    // With the empty name each category looks to the environment for its
    // own, so they can come out different.
    for (int i = 0; i < NUM_LOCALE_CATEGORIES; ++i)
      if (!codeset_is_supported(name_or_environment(i, name)))
        return nullptr;
    for (int i = 0; i < NUM_LOCALE_CATEGORIES; ++i)
      store_name(i, name_or_environment(i, name));
    return current_locale_name(LC_ALL);
  }

  if (category < 0 || category >= NUM_LOCALE_CATEGORIES)
    return nullptr;
  cpp::string_view chosen = name_or_environment(category, name);
  if (!codeset_is_supported(chosen))
    return nullptr;
  store_name(category, chosen);
  return current[category].name;
}

const char *current_codeset() {
  return current[LC_CTYPE].utf8 ? "UTF-8" : "ANSI_X3.4-1968";
}

} // namespace internal

} // namespace LIBC_NAMESPACE_DECL
