//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the reader of GNU message catalogs.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC___SUPPORT_LIBINTL_CATALOG_H
#define LLVM_LIBC_SRC___SUPPORT_LIBINTL_CATALOG_H

#include "src/__support/CPP/string_view.h"
#include "src/__support/common.h"
#include "src/__support/libintl/plural.h"
#include "src/__support/macros/config.h"

#include <stddef.h>
#include <stdint.h>

namespace LIBC_NAMESPACE_DECL {
namespace libintl {

// A catalog held in memory, which is how it is read: the file is mapped and
// nothing is copied out of it. The format is a header, then a table of the
// lengths and offsets of the original messages in sorted order, then the same
// for their translations. A message with plural forms has the singular and the
// plural joined by a null as its original, and the forms of its translation
// joined the same way.
class MessageCatalog {
  const unsigned char *data = nullptr;
  size_t size = 0;
  bool swapped = false;
  uint32_t count = 0;
  size_t originals = 0;
  size_t translations = 0;
  PluralRule rule;
  cpp::string_view header_text;

  static constexpr uint32_t MAGIC = 0x950412de;
  static constexpr size_t HEADER_SIZE = 28;

  LIBC_INLINE uint32_t word(size_t offset) const {
    uint32_t v = static_cast<uint32_t>(data[offset]) |
                 static_cast<uint32_t>(data[offset + 1]) << 8 |
                 static_cast<uint32_t>(data[offset + 2]) << 16 |
                 static_cast<uint32_t>(data[offset + 3]) << 24;
    if (!swapped)
      return v;
    return (v >> 24) | ((v >> 8) & 0xff00) | ((v << 8) & 0xff0000) | (v << 24);
  }

  // The string an entry of one of the two tables points at, which has to lie
  // inside the file and end in a null just after its length.
  LIBC_INLINE bool entry(size_t table, uint32_t index, const char *&str,
                         uint32_t &len) const {
    size_t at = table + static_cast<size_t>(index) * 8;
    len = word(at);
    uint32_t offset = word(at + 4);
    if (offset > size || len >= size - offset)
      return false;
    if (data[offset + len] != '\0')
      return false;
    str = reinterpret_cast<const char *>(data + offset);
    return true;
  }

  // Orders a message against the original at an index, up to the null which
  // ends the singular.
  LIBC_INLINE int compare(const char *msgid, const char *original) const {
    for (;; ++msgid, ++original) {
      unsigned char a = static_cast<unsigned char>(*msgid);
      unsigned char b = static_cast<unsigned char>(*original);
      if (a != b)
        return a < b ? -1 : 1;
      if (a == '\0')
        return 0;
    }
  }

  LIBC_INLINE bool find(const char *msgid, uint32_t &index) const {
    uint32_t low = 0;
    uint32_t high = count;
    while (low < high) {
      uint32_t mid = low + (high - low) / 2;
      const char *original;
      uint32_t len;
      if (!entry(originals, mid, original, len))
        return false;
      int order = compare(msgid, original);
      if (order == 0) {
        index = mid;
        return true;
      }
      if (order < 0)
        high = mid;
      else
        low = mid + 1;
    }
    return false;
  }

public:
  // Takes the bytes of a catalog and checks that its tables lie inside them.
  // The bytes have to outlive the catalog.
  LIBC_INLINE bool load(const void *bytes, size_t length) {
    data = static_cast<const unsigned char *>(bytes);
    size = length;
    if (size < HEADER_SIZE)
      return false;
    uint32_t magic = static_cast<uint32_t>(data[0]) |
                     static_cast<uint32_t>(data[1]) << 8 |
                     static_cast<uint32_t>(data[2]) << 16 |
                     static_cast<uint32_t>(data[3]) << 24;
    if (magic == MAGIC)
      swapped = false;
    else if (magic == 0xde120495)
      swapped = true;
    else
      return false;
    // Revision 1 adds tables of messages with system dependent parts, such as
    // the macros for the size of an integer. Those are not read, and the rest
    // of a revision 1 catalog is the same as revision 0.
    if ((word(4) >> 16) > 1)
      return false;
    count = word(8);
    originals = word(12);
    translations = word(16);
    size_t table_size = static_cast<size_t>(count) * 8;
    if (originals > size || translations > size ||
        table_size > size - originals || table_size > size - translations)
      return false;
    const char *header;
    uint32_t index;
    if (find("", index)) {
      uint32_t len;
      if (entry(translations, index, header, len)) {
        header_text = cpp::string_view(header, len);
        rule = PluralRule(header_text);
      }
    }
    return true;
  }

  // The header entry, which names the character set the messages are in.
  LIBC_INLINE cpp::string_view header() const { return header_text; }

  // The translation of a message, or nullptr where the catalog does not have
  // one. For a message with plural forms the form is chosen by the count.
  LIBC_INLINE const char *translate(const char *msgid, bool plural,
                                    unsigned long n) const {
    uint32_t index;
    if (!find(msgid, index))
      return nullptr;
    const char *forms;
    uint32_t len;
    if (!entry(translations, index, forms, len))
      return nullptr;
    unsigned long wanted = plural ? rule.form(n) : 0;
    const char *end = forms + len;
    const char *at = forms;
    for (unsigned long i = 0; i < wanted; ++i) {
      while (at < end && *at != '\0')
        ++at;
      if (at >= end)
        return nullptr;
      ++at;
    }
    return at;
  }
};

} // namespace libintl
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC___SUPPORT_LIBINTL_CATALOG_H
