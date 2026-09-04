//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Shared helpers for the mntent family.
///
//===----------------------------------------------------------------------===//

#include "src/mntent/mntent_utils.h"

#include "src/__support/File/file.h"
#include "src/__support/ctype_utils.h"
#include "src/__support/macros/config.h"
#include "src/__support/str_to_integer.h"

namespace LIBC_NAMESPACE_DECL {
namespace mntent {

namespace {

LIBC_INLINE bool is_blank(char c) { return c == ' ' || c == '\t'; }

// A field may hold a space, a tab, a newline or a backslash only as a three
// digit octal escape, which is undone here in place.
void decode(char *field) {
  char *out = field;
  for (const char *in = field; *in != '\0'; ++in) {
    if (in[0] == '\\' && internal::isdigit(in[1]) && internal::isdigit(in[2]) &&
        internal::isdigit(in[3]) && in[1] < '8' && in[2] < '8' && in[3] < '8') {
      *out++ = static_cast<char>(((in[1] - '0') << 6) | ((in[2] - '0') << 3) |
                                 (in[3] - '0'));
      in += 3;
      continue;
    }
    *out++ = *in;
  }
  *out = '\0';
}

// Returns the next whitespace separated field, terminating it in place, or
// nullptr once the line is used up.
char *next_field(char **rest) {
  char *p = *rest;
  while (is_blank(*p))
    ++p;
  if (*p == '\0' || *p == '\n')
    return nullptr;

  char *start = p;
  while (*p != '\0' && *p != '\n' && !is_blank(*p))
    ++p;
  if (*p != '\0') {
    *p = '\0';
    ++p;
  }
  *rest = p;
  decode(start);
  return start;
}

int to_int(const char *field) {
  if (field == nullptr)
    return 0;
  auto res = internal::strtointeger<int>(field, 10);
  if (res.has_error() || res.parsed_len <= 0)
    return 0;
  return res.value;
}

} // anonymous namespace

bool parse_line(char *line, struct mntent *entry) {
  if (line == nullptr || entry == nullptr)
    return false;

  char *rest = line;
  while (is_blank(*rest))
    ++rest;
  // Blank lines and comments are skipped rather than reported as an error.
  if (*rest == '\0' || *rest == '\n' || *rest == '#')
    return false;

  entry->mnt_fsname = next_field(&rest);
  entry->mnt_dir = next_field(&rest);
  entry->mnt_type = next_field(&rest);
  entry->mnt_opts = next_field(&rest);
  if (entry->mnt_fsname == nullptr || entry->mnt_dir == nullptr ||
      entry->mnt_type == nullptr || entry->mnt_opts == nullptr)
    return false;

  // The dump frequency and fsck pass may both be left off, and are zero then.
  entry->mnt_freq = to_int(next_field(&rest));
  entry->mnt_passno = to_int(next_field(&rest));
  return true;
}

namespace {

// Writes |name| with each character which would otherwise separate fields
// replaced by its three digit octal escape.
bool write_encoded(File *file, const char *name) {
  if (name == nullptr)
    name = "";
  for (const char *p = name; *p != '\0'; ++p) {
    const char *escape = nullptr;
    switch (*p) {
    case ' ':
      escape = "\\040";
      break;
    case '\t':
      escape = "\\011";
      break;
    case '\n':
      escape = "\\012";
      break;
    case '\\':
      escape = "\\134";
      break;
    default:
      break;
    }
    if (escape != nullptr) {
      if (file->write_unlocked(escape, 4).has_error())
        return false;
      continue;
    }
    if (file->write_unlocked(p, 1).has_error())
      return false;
  }
  return true;
}

// Writes |value| in decimal. Only the small non-negative dump frequency and
// pass number go through here.
bool write_int(File *file, int value) {
  char digits[16];
  size_t len = 0;
  bool negative = value < 0;
  unsigned magnitude = negative ? 0u - static_cast<unsigned>(value)
                                : static_cast<unsigned>(value);
  do {
    digits[len++] = static_cast<char>('0' + magnitude % 10);
    magnitude /= 10;
  } while (magnitude != 0);
  if (negative)
    digits[len++] = '-';

  char out[16];
  for (size_t i = 0; i < len; ++i)
    out[i] = digits[len - 1 - i];
  return !file->write_unlocked(out, len).has_error();
}

} // anonymous namespace

bool write_entry(void *f, const struct mntent *entry) {
  auto *file = reinterpret_cast<File *>(f);

  const char *const fields[4] = {entry->mnt_fsname, entry->mnt_dir,
                                 entry->mnt_type, entry->mnt_opts};
  for (const char *field : fields) {
    if (!write_encoded(file, field))
      return false;
    if (file->write_unlocked(" ", 1).has_error())
      return false;
  }
  if (!write_int(file, entry->mnt_freq))
    return false;
  if (file->write_unlocked(" ", 1).has_error())
    return false;
  if (!write_int(file, entry->mnt_passno))
    return false;
  return !file->write_unlocked("\n", 1).has_error();
}

} // namespace mntent
} // namespace LIBC_NAMESPACE_DECL
