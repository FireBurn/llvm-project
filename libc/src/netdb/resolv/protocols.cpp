//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Reading the protocols database
///
//===----------------------------------------------------------------------===//

#include "src/netdb/resolv/protocols.h"

#include "src/__support/CPP/string_view.h"
#include "src/__support/macros/attributes.h"
#include "src/__support/macros/config.h"
#include "src/__support/nss/dispatch.h"
#include "src/__support/nss/lookup.h"
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
LIBC_THREAD_LOCAL nss::Cursor cursor;

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

// What a module put in the storage, as the answer.
nss::Status offered(nss::Status status, const ProtoStorage &storage) {
  if (status != nss::SUCCESS)
    return status;
  return storage.entry.p_name != nullptr ? nss::SUCCESS : nss::NOTFOUND;
}

struct protoent *file_by_name(const char *name, ProtoStorage &storage) {
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

struct protoent *file_by_number(int number, ProtoStorage &storage) {
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

} // anonymous namespace

struct protoent *by_name(const char *name, ProtoStorage &storage) {
  if (name == nullptr)
    return nullptr;
  auto status = nss::lookup(
      nss::PROTOCOLS, "getprotobyname_r",
      [&] {
        return file_by_name(name, storage) != nullptr ? nss::SUCCESS
                                                      : nss::NOTFOUND;
      },
      [] { return nss::UNAVAIL; },
      [&](void *fn) {
        storage.entry.p_name = nullptr;
        int error = 0;
        return offered(
            nss::call_get<struct protoent, const char *>(
                fn, name, &storage.entry,
                cpp::span<char>(storage.pool, ProtoStorage::POOL_SIZE), error),
            storage);
      });
  return status == nss::SUCCESS ? &storage.entry : nullptr;
}

struct protoent *by_number(int number, ProtoStorage &storage) {
  auto status = nss::lookup(
      nss::PROTOCOLS, "getprotobynumber_r",
      [&] {
        return file_by_number(number, storage) != nullptr ? nss::SUCCESS
                                                          : nss::NOTFOUND;
      },
      [] { return nss::UNAVAIL; },
      [&](void *fn) {
        storage.entry.p_name = nullptr;
        int error = 0;
        return offered(
            nss::call_get<struct protoent, int>(
                fn, number, &storage.entry,
                cpp::span<char>(storage.pool, ProtoStorage::POOL_SIZE), error),
            storage);
      });
  return status == nss::SUCCESS ? &storage.entry : nullptr;
}

void rewind_entries(bool stay_open) {
  nss::reset(cursor, "endprotoent");
  enumeration.rewind(PROTOCOLS_PATH, stay_open);
}

void stop_entries() {
  nss::reset(cursor, "endprotoent");
  enumeration.stop();
}

struct protoent *next_entry(ProtoStorage &storage) {
  auto status = nss::next_entry(
      cursor, nss::PROTOCOLS, "setprotoent", "getprotoent_r", "endprotoent",
      [&] {
        char line[512];
        while (enumeration.next(PROTOCOLS_PATH, line, sizeof(line)))
          if (parse(line, storage))
            return nss::SUCCESS;
        return nss::NOTFOUND;
      },
      [&](void *fn, bool is_control) {
        if (is_control)
          return nss::call_control(fn);
        storage.entry.p_name = nullptr;
        int error = 0;
        return offered(
            nss::call_next<struct protoent>(
                fn, &storage.entry,
                cpp::span<char>(storage.pool, ProtoStorage::POOL_SIZE), error),
            storage);
      });
  return status == nss::SUCCESS ? &storage.entry : nullptr;
}

} // namespace resolv
} // namespace LIBC_NAMESPACE_DECL
