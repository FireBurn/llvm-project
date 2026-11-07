//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Reads the file that tells which SELinux context holds a system property.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_SYS_SYSTEM_PROPERTIES_PROPERTY_INFO_H
#define LLVM_LIBC_SRC_SYS_SYSTEM_PROPERTIES_PROPERTY_INFO_H

#include "src/__support/macros/config.h"
#include "src/string/string_utils.h"

#include <stddef.h>
#include <stdint.h>

namespace LIBC_NAMESPACE_DECL {
namespace system_properties {

// The process that serves the properties writes this file, and every program
// that reads one maps it. Its layout is the one of the Android property info
// area. Everything in it refers to other things by their offset from its
// start, and none of it is trusted: an offset past the end is an absent entry.
constexpr uint32_t NO_INDEX = ~0u;

class PropertyInfo {
public:
  LIBC_INLINE PropertyInfo() = default;
  LIBC_INLINE PropertyInfo(const void *data, size_t size)
      : base_(static_cast<const char *>(data)), size_(size) {}

  // False for something that is not a property info area, or is too short to
  // hold the one it says it is.
  LIBC_INLINE bool valid() const {
    if (base_ == nullptr || size_ < sizeof(Header))
      return false;
    const Header *header = this->header();
    return header->minimum_supported_version <= SUPPORTED_VERSION &&
           header->size <= size_ && contexts() != nullptr && root() != nullptr;
  }

  LIBC_INLINE uint32_t num_contexts() const {
    const uint32_t *array = contexts();
    return array == nullptr ? 0 : array[0];
  }

  // The name of a context, or null for an index there is none for.
  LIBC_INLINE const char *context(uint32_t index) const {
    const uint32_t *array = contexts();
    if (array == nullptr || index >= array[0])
      return nullptr;
    return string(array[1 + index]);
  }

  // The index of the context a property belongs to, or NO_INDEX.
  LIBC_INLINE uint32_t context_of(const char *name) const {
    uint32_t found = NO_INDEX;
    const Node *node = root();
    if (node == nullptr)
      return found;
    const char *remaining = name;
    for (;;) {
      const char *dot = find_dot(remaining);
      take_context(entry(node->property_entry), found);
      take_prefix(node, remaining, found);
      if (dot == nullptr)
        break;
      node = child(node, remaining, static_cast<uint32_t>(dot - remaining));
      if (node == nullptr)
        return found;
      remaining = dot + 1;
    }
    // At the node of the last part of the name, an exact match wins over any
    // prefix, and prefixes that do not end at a dot are looked at last.
    const uint32_t *exact =
        array(node->exact_match_entries, node->num_exact_matches);
    for (uint32_t i = 0; exact != nullptr && i < node->num_exact_matches; ++i) {
      const Entry *e = entry(exact[i]);
      const char *e_name = e == nullptr ? nullptr : string(e->name_offset);
      if (e_name != nullptr && compare(e_name, remaining) == 0) {
        if (e->context_index != NO_INDEX)
          found = e->context_index;
        return found;
      }
    }
    take_prefix(node, remaining, found);
    return found;
  }

private:
  struct Header {
    uint32_t current_version;
    uint32_t minimum_supported_version;
    uint32_t size;
    uint32_t contexts_offset;
    uint32_t types_offset;
    uint32_t root_offset;
  };
  struct Entry {
    uint32_t name_offset;
    uint32_t name_length;
    uint32_t context_index;
    uint32_t type_index;
  };
  struct Node {
    uint32_t property_entry;
    uint32_t num_child_nodes;
    uint32_t child_nodes;
    uint32_t num_prefixes;
    uint32_t prefix_entries;
    uint32_t num_exact_matches;
    uint32_t exact_match_entries;
  };

  static constexpr uint32_t SUPPORTED_VERSION = 1;

  const char *base_ = nullptr;
  size_t size_ = 0;

  LIBC_INLINE const Header *header() const {
    return reinterpret_cast<const Header *>(base_);
  }

  // The object of the given size at the offset, if all of it is in the area.
  LIBC_INLINE const char *at(uint32_t offset, size_t bytes) const {
    if (offset > size_ || bytes > size_ - offset || (offset & 3) != 0)
      return nullptr;
    return base_ + offset;
  }

  // A string, which has to end before the end of the area.
  LIBC_INLINE const char *string(uint32_t offset) const {
    if (offset >= size_)
      return nullptr;
    const char *text = base_ + offset;
    for (size_t left = size_ - offset; left > 0; --left, ++text)
      if (*text == '\0')
        return base_ + offset;
    return nullptr;
  }

  LIBC_INLINE const uint32_t *array(uint32_t offset, uint32_t count) const {
    if (count > size_ / sizeof(uint32_t))
      return nullptr;
    return reinterpret_cast<const uint32_t *>(
        at(offset, count * sizeof(uint32_t)));
  }

  // The array with its length in front, which holds the contexts.
  LIBC_INLINE const uint32_t *contexts() const {
    const uint32_t *counted = reinterpret_cast<const uint32_t *>(
        at(header()->contexts_offset, sizeof(uint32_t)));
    if (counted == nullptr)
      return nullptr;
    return array(header()->contexts_offset, 1 + counted[0]);
  }

  LIBC_INLINE const Entry *entry(uint32_t offset) const {
    return reinterpret_cast<const Entry *>(at(offset, sizeof(Entry)));
  }
  LIBC_INLINE const Node *node(uint32_t offset) const {
    return reinterpret_cast<const Node *>(at(offset, sizeof(Node)));
  }
  LIBC_INLINE const Node *root() const { return node(header()->root_offset); }

  LIBC_INLINE static const char *find_dot(const char *text) {
    for (; *text != '\0'; ++text)
      if (*text == '.')
        return text;
    return nullptr;
  }

  LIBC_INLINE static int compare(const char *a, const char *b) {
    for (; *a != '\0' && *a == *b; ++a, ++b) {
    }
    return static_cast<unsigned char>(*a) - static_cast<unsigned char>(*b);
  }

  LIBC_INLINE void take_context(const Entry *e, uint32_t &found) const {
    if (e != nullptr && e->context_index != NO_INDEX)
      found = e->context_index;
  }

  // The first prefix, the longest being first, the rest of the name starts
  // with.
  LIBC_INLINE void take_prefix(const Node *n, const char *remaining,
                               uint32_t &found) const {
    const uint32_t *prefixes = array(n->prefix_entries, n->num_prefixes);
    const size_t remaining_length = internal::string_length(remaining);
    for (uint32_t i = 0; prefixes != nullptr && i < n->num_prefixes; ++i) {
      const Entry *e = entry(prefixes[i]);
      if (e == nullptr || e->name_length > remaining_length)
        continue;
      const char *prefix = string(e->name_offset);
      if (prefix == nullptr)
        continue;
      bool same = true;
      for (uint32_t k = 0; k < e->name_length && same; ++k)
        same = prefix[k] == remaining[k];
      if (same) {
        if (e->context_index != NO_INDEX)
          found = e->context_index;
        return;
      }
    }
  }

  // The child that is named by the first length characters of the text.
  LIBC_INLINE const Node *child(const Node *parent, const char *text,
                                uint32_t length) const {
    const uint32_t *children =
        array(parent->child_nodes, parent->num_child_nodes);
    for (uint32_t i = 0; children != nullptr && i < parent->num_child_nodes;
         ++i) {
      const Node *candidate = node(children[i]);
      const Entry *e =
          candidate == nullptr ? nullptr : entry(candidate->property_entry);
      if (e == nullptr || e->name_length != length)
        continue;
      const char *name = string(e->name_offset);
      if (name == nullptr)
        continue;
      bool same = true;
      for (uint32_t k = 0; k < length && same; ++k)
        same = name[k] == text[k];
      if (same)
        return candidate;
    }
    return nullptr;
  }
};

} // namespace system_properties
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_SYS_SYSTEM_PROPERTIES_PROPERTY_INFO_H
