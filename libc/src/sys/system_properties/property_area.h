//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// The shared memory layout of the system properties.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_SYS_SYSTEM_PROPERTIES_PROPERTY_AREA_H
#define LLVM_LIBC_SRC_SYS_SYSTEM_PROPERTIES_PROPERTY_AREA_H

#include "src/__support/CPP/atomic.h"
#include "src/__support/macros/config.h"
#include "src/string/memory_utils/inline_memcpy.h"
#include "src/string/string_utils.h"

#include <stddef.h>
#include <stdint.h>

namespace LIBC_NAMESPACE_DECL {
namespace system_properties {

// The values are those of the Android property area, which other programs
// that map it rely on.
constexpr size_t VALUE_MAX = 92;
constexpr size_t AREA_SIZE = 128 * 1024;
constexpr uint32_t AREA_MAGIC = 0x504f5250;
constexpr uint32_t AREA_VERSION = 0xfc6ed0ab;
constexpr uint32_t LONG_FLAG = 1 << 16;
constexpr size_t LONG_ERROR_SIZE = 56;
constexpr char LONG_MESSAGE[] = "Must use __system_property_read_callback() to read";

using Word = cpp::Atomic<uint32_t>;

// The serial of a property holds the length of its value in the top byte, the
// flags below that, and in its bottom bit whether it is being changed.
LIBC_INLINE constexpr uint32_t serial_value_length(uint32_t serial) {
  return serial >> 24;
}
LIBC_INLINE constexpr bool serial_dirty(uint32_t serial) {
  return (serial & 1) != 0;
}

LIBC_INLINE constexpr size_t align_up(size_t value) {
  return (value + sizeof(uint32_t) - 1) & ~(sizeof(uint32_t) - 1);
}

// A property. The name follows it, and after that the value when that is too
// long to fit in the space here.
struct PropInfo {
  Word serial;
  union {
    char value[VALUE_MAX];
    struct {
      char error_message[LONG_ERROR_SIZE];
      uint32_t offset;
    } long_property;
  };

  LIBC_INLINE const char *name() const {
    return reinterpret_cast<const char *>(this) + sizeof(PropInfo);
  }
  LIBC_INLINE bool is_long() const {
    return (const_cast<Word &>(serial).load(cpp::MemoryOrder::RELAXED) &
            LONG_FLAG) != 0;
  }
  LIBC_INLINE const char *long_value() const {
    // Not a pointer, as the area is mapped at a different address in every
    // process.
    return reinterpret_cast<const char *>(this) + long_property.offset;
  }
};
static_assert(sizeof(PropInfo) == 96, "the property layout is fixed");

// Properties are held in a trie split at the dots of their names, with the
// siblings at each level in a binary tree. Links are offsets into the area.
struct TrieNode {
  uint32_t namelen;
  Word prop;
  Word left;
  Word right;
  Word children;

  LIBC_INLINE char *name() {
    return reinterpret_cast<char *>(this) + sizeof(TrieNode);
  }
};
static_assert(sizeof(TrieNode) == 20, "the trie layout is fixed");

class Area {
public:
  LIBC_INLINE void init() {
    magic_ = AREA_MAGIC;
    version_ = AREA_VERSION;
    serial_.store(0, cpp::MemoryOrder::RELAXED);
    for (uint32_t &word : reserved_)
      word = 0;
    // The root node, and then the dirty backup area readers fall back on while
    // a value is being changed.
    bytes_used_ = sizeof(TrieNode) + align_up(VALUE_MAX);
  }

  LIBC_INLINE bool valid() const {
    return magic_ == AREA_MAGIC && version_ == AREA_VERSION;
  }

  LIBC_INLINE Word &serial() { return serial_; }
  LIBC_INLINE char *dirty_backup_area() { return data() + sizeof(TrieNode); }

  LIBC_INLINE const PropInfo *find(const char *name) {
    return find_property(root(), name, internal::string_length(name), nullptr,
                         0, false);
  }

  LIBC_INLINE bool add(const char *name, size_t name_length,
                       const char *value, size_t value_length) {
    return find_property(root(), name, name_length, value, value_length,
                         true) != nullptr;
  }

  // Calls the function for every property, in the order of the trie.
  template <typename Function> LIBC_INLINE bool foreach (Function function) {
    return foreach_property(root(), function);
  }

private:
  LIBC_INLINE char *data() { return reinterpret_cast<char *>(this + 1); }
  LIBC_INLINE TrieNode *root() { return reinterpret_cast<TrieNode *>(data()); }

  LIBC_INLINE void *to_object(uint32_t offset) {
    if (offset >= AREA_SIZE - sizeof(Area))
      return nullptr;
    return data() + offset;
  }
  LIBC_INLINE TrieNode *to_node(Word &link) {
    return static_cast<TrieNode *>(
        to_object(link.load(cpp::MemoryOrder::ACQUIRE)));
  }
  LIBC_INLINE PropInfo *to_property(Word &link) {
    return static_cast<PropInfo *>(
        to_object(link.load(cpp::MemoryOrder::ACQUIRE)));
  }

  LIBC_INLINE void *allocate(size_t size, uint32_t &offset) {
    size = align_up(size);
    if (bytes_used_ + size > AREA_SIZE - sizeof(Area))
      return nullptr;
    offset = bytes_used_;
    bytes_used_ += static_cast<uint32_t>(size);
    return data() + offset;
  }

  LIBC_INLINE TrieNode *new_node(const char *name, uint32_t name_length,
                                 uint32_t &offset) {
    void *memory = allocate(sizeof(TrieNode) + name_length + 1, offset);
    if (memory == nullptr)
      return nullptr;
    TrieNode *node = static_cast<TrieNode *>(memory);
    node->namelen = name_length;
    inline_memcpy(node->name(), name, name_length);
    node->name()[name_length] = '\0';
    return node;
  }

  LIBC_INLINE PropInfo *new_property(const char *name, uint32_t name_length,
                                     const char *value, uint32_t value_length,
                                     uint32_t &offset) {
    const size_t head = sizeof(PropInfo) + name_length + 1;
    const bool is_long = value_length >= VALUE_MAX;
    void *memory = allocate(is_long ? align_up(head) + value_length + 1 : head,
                            offset);
    if (memory == nullptr)
      return nullptr;
    PropInfo *info = static_cast<PropInfo *>(memory);
    inline_memcpy(const_cast<char *>(info->name()), name, name_length);
    const_cast<char *>(info->name())[name_length] = '\0';
    if (!is_long) {
      inline_memcpy(info->value, value, value_length);
      info->value[value_length] = '\0';
      info->serial.store(value_length << 24, cpp::MemoryOrder::RELAXED);
      return info;
    }
    const uint32_t value_offset = static_cast<uint32_t>(align_up(head));
    inline_memcpy(reinterpret_cast<char *>(info) + value_offset, value,
                  value_length);
    reinterpret_cast<char *>(info)[value_offset + value_length] = '\0';
    inline_memcpy(info->long_property.error_message, LONG_MESSAGE,
                  sizeof(LONG_MESSAGE));
    info->long_property.offset = value_offset;
    info->serial.store((value_length << 24) | LONG_FLAG,
                       cpp::MemoryOrder::RELAXED);
    return info;
  }

  // Shorter names go first, then they are compared as bytes.
  LIBC_INLINE static int compare(const char *one, uint32_t one_length,
                                 const char *two, uint32_t two_length) {
    if (one_length < two_length)
      return -1;
    if (one_length > two_length)
      return 1;
    for (uint32_t i = 0; i < one_length; ++i)
      if (one[i] != two[i])
        return static_cast<unsigned char>(one[i]) <
                       static_cast<unsigned char>(two[i])
                   ? -1
                   : 1;
    return 0;
  }

  LIBC_INLINE TrieNode *find_node(TrieNode *trie, const char *name,
                                  uint32_t name_length, bool allocate_missing) {
    TrieNode *current = trie;
    while (current != nullptr) {
      const int order = compare(current->name(), current->namelen, name,
                                name_length);
      if (order == 0)
        return current;
      Word &link = order < 0 ? current->left : current->right;
      if (link.load(cpp::MemoryOrder::RELAXED) != 0) {
        current = to_node(link);
        continue;
      }
      if (!allocate_missing)
        return nullptr;
      uint32_t offset;
      TrieNode *created = new_node(name, name_length, offset);
      if (created != nullptr)
        link.store(offset, cpp::MemoryOrder::RELEASE);
      return created;
    }
    return nullptr;
  }

  LIBC_INLINE const PropInfo *find_property(TrieNode *trie, const char *name,
                                            size_t name_length,
                                            const char *value,
                                            size_t value_length,
                                            bool allocate_missing) {
    TrieNode *current = trie;
    const char *remaining = name;
    for (;;) {
      const char *dot = remaining;
      while (*dot != '\0' && *dot != '.')
        ++dot;
      const bool more = *dot == '.';
      const uint32_t part = static_cast<uint32_t>(dot - remaining);
      if (part == 0)
        return nullptr;

      TrieNode *level = nullptr;
      if (current->children.load(cpp::MemoryOrder::RELAXED) != 0) {
        level = to_node(current->children);
      } else if (allocate_missing) {
        uint32_t offset;
        level = new_node(remaining, part, offset);
        if (level != nullptr)
          current->children.store(offset, cpp::MemoryOrder::RELEASE);
      }
      if (level == nullptr)
        return nullptr;
      current = find_node(level, remaining, part, allocate_missing);
      if (current == nullptr)
        return nullptr;
      if (!more)
        break;
      remaining = dot + 1;
    }

    if (current->prop.load(cpp::MemoryOrder::RELAXED) != 0)
      return to_property(current->prop);
    if (!allocate_missing)
      return nullptr;
    uint32_t offset;
    PropInfo *created =
        new_property(name, static_cast<uint32_t>(name_length), value,
                     static_cast<uint32_t>(value_length), offset);
    if (created != nullptr)
      current->prop.store(offset, cpp::MemoryOrder::RELEASE);
    return created;
  }

  template <typename Function>
  LIBC_INLINE bool foreach_property(TrieNode *trie, Function &function) {
    if (trie == nullptr)
      return true;
    if (trie->left.load(cpp::MemoryOrder::RELAXED) != 0 &&
        !foreach_property(to_node(trie->left), function))
      return false;
    if (trie->prop.load(cpp::MemoryOrder::RELAXED) != 0) {
      const PropInfo *info = to_property(trie->prop);
      if (info == nullptr)
        return false;
      function(info);
    }
    if (trie->children.load(cpp::MemoryOrder::RELAXED) != 0 &&
        !foreach_property(to_node(trie->children), function))
      return false;
    if (trie->right.load(cpp::MemoryOrder::RELAXED) != 0 &&
        !foreach_property(to_node(trie->right), function))
      return false;
    return true;
  }

  uint32_t bytes_used_;
  Word serial_;
  uint32_t magic_;
  uint32_t version_;
  uint32_t reserved_[28];
};
static_assert(sizeof(Area) == 128, "the area header layout is fixed");

} // namespace system_properties
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_SYS_SYSTEM_PROPERTIES_PROPERTY_AREA_H
