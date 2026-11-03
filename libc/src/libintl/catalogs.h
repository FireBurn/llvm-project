//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the declaration of the registry of text domains.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_LIBINTL_CATALOGS_H
#define LLVM_LIBC_SRC_LIBINTL_CATALOGS_H

#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {
namespace libintl {

// The domain gettext looks in when it is not told one.
const char *current_domain();

// Makes a domain the current one; an empty name means "messages". Returns the
// name now in force, or nullptr where there was no memory to keep it.
const char *set_current_domain(const char *name);

// Where a domain's catalogs are looked for. Both return the directory in
// force, and the setter returns nullptr where there was no memory to keep it.
const char *domain_directory(const char *domain);
const char *set_domain_directory(const char *domain, const char *directory);

// The character set a domain was asked to give its messages in, or nullptr
// where it was not asked. The setter returns it as stored.
const char *domain_codeset(const char *domain);
const char *set_domain_codeset(const char *domain, const char *codeset);

// The translation of a message in the catalog of the domain for a locale, in
// the character set the domain asked for or the locale uses, or nullptr where
// there is none. With a plural, the form for the count is chosen.
const char *translate(const char *domain, const char *msgid, bool plural,
                      unsigned long count, int category,
                      const char *locale_name);

// What gettext returns for a message: its translation where there is one, and
// otherwise the message itself, the singular or the plural by the count in the
// English way. A null domain is the current one and a null plural is a message
// without forms.
const char *lookup(const char *domain, const char *msgid1, const char *msgid2,
                   unsigned long count, int category, const char *locale_name);

} // namespace libintl
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_LIBINTL_CATALOGS_H
