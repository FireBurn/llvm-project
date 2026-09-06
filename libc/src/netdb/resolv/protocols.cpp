//===-- Reading the protocols database ------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/netdb/resolv/protocols.h"

#include "src/__support/CPP/string_view.h"
#include "src/__support/macros/attributes.h"
#include "src/__support/macros/config.h"
#include "src/__support/str_to_integer.h"
#include "src/netdb/resolv/file_lines.h"
#include "src/string/memory_utils/inline_memcpy.h"

namespace LIBC_NAMESPACE_DECL {
namespace resolv {

namespace {

constexpr const char *PROTOCOLS_PATH = "/etc/protocols";

// Where the enumeration has got to. setprotoent asks for the file to be held
// open across lookups, which is what the enumeration records.
LIBC_CONSTINIT LIBC_THREAD_LOCAL Enumeration enumeration;

// Copies a field into the storage's pool and hands back a pointer to it, or
// null where there is no room left.
char *keep(cpp::string_view field, ProtoStorage &storage) {
  if (storage.used + field.size() + 1 > ProtoStorage::POOL_SIZE)
    return nullptr;
  char *at = storage.pool + storage.used;
  inline_memcpy(at, field.data(), field.size());
  at[field.size()] = '\0';
  storage.used += field.size() + 1;
  return at;
}

// Reads one line of the file into `storage`. Returns false where the line
// holds no entry, which a comment or a blank line does not.
bool parse(char *line, ProtoStorage &storage) {
  drop_comment(line);

  // A name, then the number, then any other names it goes by.
  cpp::string_view rest(line);
  cpp::string_view official = next_field(rest);
  if (official.empty())
    return false;
  cpp::string_view digits = next_field(rest);
  if (digits.empty())
    return false;

  char number[16];
  if (digits.size() >= sizeof(number))
    return false;
  inline_memcpy(number, digits.data(), digits.size());
  number[digits.size()] = '\0';
  auto value = internal::strtointeger<long>(number, 10);
  // An IP header carries the number in one octet, but the file is not only
  // for those: entries above 255 name things that never travel in one, and
  // p_proto is wide enough to say so.
  if (value.value < 0 || value.value > 0x7fffffff)
    return false;

  storage.used = 0;
  char *name = keep(official, storage);
  if (name == nullptr)
    return false;

  size_t count = 0;
  for (cpp::string_view other = next_field(rest); !other.empty();
       other = next_field(rest)) {
    if (count == ProtoStorage::MAX_ALIASES)
      break;
    char *kept = keep(other, storage);
    if (kept == nullptr)
      break;
    storage.aliases[count++] = kept;
  }
  storage.aliases[count] = nullptr;

  storage.entry.p_name = name;
  storage.entry.p_aliases = storage.aliases;
  storage.entry.p_proto = static_cast<int>(value.value);
  return true;
}

bool names_match(const ProtoStorage &storage, const char *name) {
  cpp::string_view official(storage.entry.p_name);
  if (equal(official, name))
    return true;
  for (size_t i = 0; storage.aliases[i] != nullptr; ++i)
    if (equal(cpp::string_view(storage.aliases[i]), name))
      return true;
  return false;
}

} // anonymous namespace

struct protoent *by_name(const char *name, ProtoStorage &storage) {
  if (name == nullptr)
    return nullptr;
  FileLines reader;
  if (!reader.open(PROTOCOLS_PATH))
    return nullptr;
  struct protoent *found = nullptr;
  char line[512];
  while (found == nullptr && reader.next_line(line, sizeof(line)))
    if (parse(line, storage) && names_match(storage, name))
      found = &storage.entry;
  reader.close_file();
  return found;
}

struct protoent *by_number(int number, ProtoStorage &storage) {
  FileLines reader;
  if (!reader.open(PROTOCOLS_PATH))
    return nullptr;
  struct protoent *found = nullptr;
  char line[512];
  while (found == nullptr && reader.next_line(line, sizeof(line)))
    if (parse(line, storage) && storage.entry.p_proto == number)
      found = &storage.entry;
  reader.close_file();
  return found;
}

void rewind_entries(bool stay_open) {
  enumeration.rewind(PROTOCOLS_PATH, stay_open);
}

void stop_entries() { enumeration.stop(); }

struct protoent *next_entry(ProtoStorage &storage) {
  char line[512];
  while (enumeration.next(PROTOCOLS_PATH, line, sizeof(line)))
    if (parse(line, storage))
      return &storage.entry;
  return nullptr;
}

} // namespace resolv
} // namespace LIBC_NAMESPACE_DECL
