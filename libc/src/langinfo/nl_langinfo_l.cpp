//===-- Implementation of nl_langinfo_l -----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/langinfo/nl_langinfo_l.h"

#include "hdr/locale_macros.h"
#include "hdr/types/locale_t.h"
#include "hdr/types/nl_item.h"
#include "hdr/types/size_t.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/langinfo/langinfo_table.h"

namespace LIBC_NAMESPACE_DECL {

namespace {

// Every item is a string constant, and POSIX says the caller must not write
// through the pointer, so handing back a pointer into read only data is
// what the const_cast is for.
char *lookup(nl_item item) {
  const int index = langinfo::index_of(item);
  const char *const *table = nullptr;
  size_t count = 0;

  switch (langinfo::category_of(item)) {
  case LC_CTYPE:
    table = langinfo::CTYPE_ITEMS;
    count = sizeof(langinfo::CTYPE_ITEMS) / sizeof(*langinfo::CTYPE_ITEMS);
    break;
  case LC_NUMERIC:
    table = langinfo::NUMERIC_ITEMS;
    count = sizeof(langinfo::NUMERIC_ITEMS) / sizeof(*langinfo::NUMERIC_ITEMS);
    break;
  case LC_TIME:
    table = langinfo::TIME_ITEMS;
    count = sizeof(langinfo::TIME_ITEMS) / sizeof(*langinfo::TIME_ITEMS);
    break;
  case LC_MONETARY:
    table = langinfo::MONETARY_ITEMS;
    count =
        sizeof(langinfo::MONETARY_ITEMS) / sizeof(*langinfo::MONETARY_ITEMS);
    break;
  case LC_MESSAGES:
    table = langinfo::MESSAGES_ITEMS;
    count =
        sizeof(langinfo::MESSAGES_ITEMS) / sizeof(*langinfo::MESSAGES_ITEMS);
    break;
  default:
    break;
  }

  // An item which is not one of these is not an error to ask about; POSIX
  // says the answer is an empty string.
  if (table == nullptr || index < 0 || static_cast<size_t>(index) >= count)
    return const_cast<char *>("");
  return const_cast<char *>(table[index]);
}

} // anonymous namespace

// Only the C locale is supported, so the locale argument makes no
// difference to the answer.
LLVM_LIBC_FUNCTION(char *, nl_langinfo_l,
                   (nl_item item, [[maybe_unused]] locale_t locale)) {
  return lookup(item);
}

} // namespace LIBC_NAMESPACE_DECL
