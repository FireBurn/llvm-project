//===-- The list of login shells ------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/unistd/usershell.h"

#include "src/__support/CPP/span.h"
#include "src/__support/flat_file_db.h"
#include "src/__support/macros/attributes.h"
#include "src/__support/macros/config.h"

#ifndef LIBC_COPT_SHELLS_FILE_PATH
#define LIBC_COPT_SHELLS_FILE_PATH "/etc/shells"
#endif

namespace LIBC_NAMESPACE_DECL {
namespace usershell {

namespace {

// A shell is a line of its own, so the database only has to hand back lines
// and nothing needs parsing into an entry.
struct Line {};

LIBC_CONSTINIT internal::FlatFileDatabase<Line> db(LIBC_COPT_SHELLS_FILE_PATH);
char line_buffer[LINE_SIZE];

// What the list is when there is no file to read, which is what every other
// implementation falls back to rather than saying there are no shells at all.
constexpr const char *DEFAULT_SHELLS[] = {"/bin/sh", "/bin/csh"};
size_t default_index = 0;
bool using_defaults = false;
bool opened = false;

// Trims the leading blanks and everything from the first blank after the
// name. Returns null for a line which names no shell.
char *shell_in(char *line) {
  while (*line == ' ' || *line == '\t')
    ++line;
  if (*line == '#' || *line == '\0' || *line == '\n')
    return nullptr;
  for (char *p = line; *p != '\0'; ++p) {
    if (*p == ' ' || *p == '\t' || *p == '\n') {
      *p = '\0';
      break;
    }
  }
  return *line == '\0' ? nullptr : line;
}

} // anonymous namespace

void TESTONLY_set_shells_path(const char *p) { db.set_path(p); }

const char *path() { return db.path(); }

void rewind() {
  db.enddb();
  default_index = 0;
  using_defaults = !db.setdb().has_value();
  opened = true;
}

void close() {
  db.enddb();
  default_index = 0;
  using_defaults = false;
  opened = false;
}

const char *next() {
  // A caller which never rewound is at the start of the list, so the file is
  // opened the first time it asks.
  if (!opened)
    rewind();

  if (using_defaults) {
    constexpr size_t COUNT = sizeof(DEFAULT_SHELLS) / sizeof(DEFAULT_SHELLS[0]);
    if (default_index >= COUNT)
      return nullptr;
    return DEFAULT_SHELLS[default_index++];
  }

  for (;;) {
    auto read = db.getline(cpp::span<char>(line_buffer, sizeof(line_buffer)));
    if (!read.has_value() || !read.value())
      return nullptr;
    if (char *shell = shell_in(line_buffer))
      return shell;
  }
}

} // namespace usershell
} // namespace LIBC_NAMESPACE_DECL
