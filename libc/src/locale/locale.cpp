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
#include "src/stdlib/getenv.h"
#include "src/stdlib/mb_cur_max.h"

namespace LIBC_NAMESPACE_DECL {

__locale_t c_locale = {nullptr};

locale_t locale = nullptr;

namespace {

// The name the locale goes by and how wide a character is in it, which is all
// of a locale that is kept.
__locale_data current = {{'C', '\0'}, false};

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

const char *locale_from_environment(int category) {
  // LC_ALL overrides the rest where it says anything at all.
  const char *all = getenv("LC_ALL");
  if (all != nullptr && *all != '\0')
    return all;

  static const char *const NAMES[NUM_LOCALE_CATEGORIES] = {
      "LC_CTYPE",   "LC_NUMERIC",  "LC_TIME",
      "LC_COLLATE", "LC_MONETARY", "LC_MESSAGES"};
  if (category >= 0 && category < NUM_LOCALE_CATEGORIES) {
    const char *named = getenv(NAMES[category]);
    if (named != nullptr && *named != '\0')
      return named;
  }

  const char *lang = getenv("LANG");
  if (lang != nullptr && *lang != '\0')
    return lang;
  return nullptr;
}

bool name_states_utf8(cpp::string_view name) {
  return same_codeset(codeset_of(name), "utf8");
}

const char *set_locale_name(int category, cpp::string_view name) {
  if (name.empty()) {
    // The empty name means whatever the environment states, and the C locale
    // where it states nothing.
    const char *from_environment = locale_from_environment(category);
    name = from_environment == nullptr ? cpp::string_view("C")
                                       : cpp::string_view(from_environment);
  }

  if (!codeset_is_supported(name))
    return nullptr;

  size_t length = name.size();
  if (length >= MAX_LOCALE_NAME_SIZE)
    length = MAX_LOCALE_NAME_SIZE - 1;
  for (size_t i = 0; i < length; ++i)
    current.name[i] = name[i];
  current.name[length] = '\0';
  current.utf8 = name_states_utf8(name);

  // How wide a character is follows from the codeset, and callers size their
  // buffers by it.
  set_mb_cur_max(current.utf8 ? 4 : 1);
  return current.name;
}

const char *current_locale_name() { return current.name; }

} // namespace internal

} // namespace LIBC_NAMESPACE_DECL
