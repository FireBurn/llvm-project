//===-- The locale in force -------------------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_LOCALE_LOCALE_H
#define LLVM_LIBC_SRC_LOCALE_LOCALE_H

#include "src/__support/CPP/string_view.h"
#include "src/__support/macros/attributes.h"
#include "src/__support/macros/config.h"

#include "hdr/types/locale_t.h"

#include <stddef.h>

namespace LIBC_NAMESPACE_DECL {

// Long enough for the names locales go by, which run to a language, a
// territory, a codeset and a modifier.
static constexpr size_t MAX_LOCALE_NAME_SIZE = 64;

struct __locale_data {
  char name[MAX_LOCALE_NAME_SIZE];
  // Whether the codeset the name states is UTF-8. Nothing else about a locale
  // is read: what is kept is the name it goes by and how wide a character is
  // in it.
  bool utf8;
};

// The pointer to the default "C" locale.
extern __locale_t c_locale;

// The global locale instance.
extern locale_t locale;

namespace internal {

// Whether a name states UTF-8 as its codeset, which is the part of it after a
// full stop, compared without regard to case or to the hyphen.
bool name_states_utf8(cpp::string_view name);

// Whether the codeset a name states is one that can be honoured. There is no
// locale database, so what a name can ask for is the character set: UTF-8, or
// the ASCII the C locale means. A name stating anything else is refused
// rather than answered with the wrong encoding.
bool codeset_is_supported(cpp::string_view name);

// What the environment says the given category should be: LC_ALL where it is
// set to anything, then the variable for the category itself, then LANG. A
// null return means none of them said.
const char *locale_from_environment(int category);

// Puts a name in place, working out from it how wide a character is. An empty
// name means the one the environment states.
//
// Returns the name settled on, which is what setlocale reports, or nullptr
// where the name states a codeset that cannot be honoured.
const char *set_locale_name(int category, cpp::string_view name);

// The name in force, which setlocale reports when asked rather than told.
const char *current_locale_name();

} // namespace internal

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_LOCALE_LOCALE_H
