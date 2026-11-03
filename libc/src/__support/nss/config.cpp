//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the reader of nsswitch.conf.
///
//===----------------------------------------------------------------------===//

#include "src/__support/nss/config.h"

#include "hdr/fcntl_macros.h"
#include "hdr/func/free.h"
#include "hdr/func/malloc.h"
#include "hdr/stdio_macros.h"
#include "hdr/types/struct_stat.h"
#include "src/__support/CPP/string_view.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/close.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/lseek.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/newfstatat.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/open.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/read.h"
#include "src/__support/macros/config.h"
#include "src/__support/threads/raw_mutex.h"

namespace LIBC_NAMESPACE_DECL {
namespace nss {

namespace {

constexpr const char *DEFAULT_PATH = "/etc/nsswitch.conf";
constexpr size_t MAX_CONFIG_SIZE = 1 << 20;

struct DatabaseName {
  const char *name;
  const char *services;
};

constexpr DatabaseName DATABASES[DATABASE_COUNT] = {
    {"passwd", "files"},    {"group", "files"},    {"shadow", "files"},
    {"hosts", "files dns"}, {"services", "files"}, {"protocols", "files"},
    {"networks", "files"},  {"ethers", "files"},   {"rpc", "files"},
    {"aliases", "files"},   {"netgroup", "files"},
};

RawMutex lock;
const char *config_path = DEFAULT_PATH;
Configuration *current = nullptr;
bool have_stamp = false;
long stamp_inode = 0;
long stamp_seconds = 0;
long stamp_nanoseconds = 0;
long stamp_size = 0;
// What __nss_configure_lookup set, which a change of the file leaves alone.
Source *overrides[DATABASE_COUNT] = {};

bool is_blank(char c) { return c == ' ' || c == '\t' || c == '\r'; }

char lower_of(char c) { return (c >= 'A' && c <= 'Z') ? c - 'A' + 'a' : c; }

bool equal_ignoring_case(cpp::string_view a, const char *b) {
  size_t i = 0;
  for (; i < a.size() && b[i] != '\0'; ++i)
    if (lower_of(a[i]) != lower_of(b[i]))
      return false;
  return i == a.size() && b[i] == '\0';
}

char *copy_of(cpp::string_view text) {
  char *copy = static_cast<char *>(::malloc(text.size() + 1));
  if (copy == nullptr)
    return nullptr;
  for (size_t i = 0; i < text.size(); ++i)
    copy[i] = text[i];
  copy[text.size()] = '\0';
  return copy;
}

cpp::string_view trimmed(cpp::string_view text) {
  while (!text.empty() && is_blank(text.front()))
    text.remove_prefix(1);
  while (!text.empty() && is_blank(text.back()))
    text.remove_suffix(1);
  return text;
}

bool status_from_name(cpp::string_view name, int &index) {
  if (equal_ignoring_case(name, "success"))
    index = SUCCESS - TRYAGAIN;
  else if (equal_ignoring_case(name, "notfound"))
    index = NOTFOUND - TRYAGAIN;
  else if (equal_ignoring_case(name, "unavail"))
    index = UNAVAIL - TRYAGAIN;
  else if (equal_ignoring_case(name, "tryagain"))
    index = TRYAGAIN - TRYAGAIN;
  else
    return false;
  return true;
}

bool action_from_name(cpp::string_view name, Action &action) {
  if (equal_ignoring_case(name, "return"))
    action = Action::RETURN;
  else if (equal_ignoring_case(name, "continue"))
    action = Action::CONTINUE;
  else if (equal_ignoring_case(name, "merge"))
    action = Action::MERGE;
  else
    return false;
  return true;
}

// Reads "[!status=action ...]", without the brackets.
void parse_actions(cpp::string_view text, Source &source) {
  while (true) {
    text = trimmed(text);
    if (text.empty())
      return;
    size_t end = 0;
    while (end < text.size() && !is_blank(text[end]))
      ++end;
    cpp::string_view item = text.substr(0, end);
    text.remove_prefix(end);
    bool negated = false;
    if (!item.empty() && item.front() == '!') {
      negated = true;
      item.remove_prefix(1);
    }
    size_t equals = 0;
    while (equals < item.size() && item[equals] != '=')
      ++equals;
    int index;
    Action action;
    if (equals == item.size() ||
        !status_from_name(item.substr(0, equals), index) ||
        !action_from_name(item.substr(equals + 1), action))
      continue;
    for (int i = 0; i < 4; ++i)
      if ((i == index) != negated)
        source.actions[i] = action;
  }
}

Source *new_source(cpp::string_view name) {
  auto *source = static_cast<Source *>(::malloc(sizeof(Source)));
  if (source == nullptr)
    return nullptr;
  source->name = copy_of(name);
  if (source->name == nullptr) {
    ::free(source);
    return nullptr;
  }
  source->actions[TRYAGAIN - TRYAGAIN] = Action::CONTINUE;
  source->actions[UNAVAIL - TRYAGAIN] = Action::CONTINUE;
  source->actions[NOTFOUND - TRYAGAIN] = Action::CONTINUE;
  source->actions[SUCCESS - TRYAGAIN] = Action::RETURN;
  source->next = nullptr;
  source->module = nullptr;
  source->module_tried = false;
  return source;
}

// Reads the words after "database:".
Source *parse_services(cpp::string_view text) {
  Source *head = nullptr;
  Source **tail = &head;
  while (true) {
    text = trimmed(text);
    if (text.empty())
      return head;
    size_t end = 0;
    while (end < text.size() && !is_blank(text[end]) && text[end] != '[')
      ++end;
    if (end == 0) {
      // A bracket with no service before it belongs to nothing.
      size_t close = 0;
      while (close < text.size() && text[close] != ']')
        ++close;
      text.remove_prefix(close < text.size() ? close + 1 : close);
      continue;
    }
    Source *source = new_source(text.substr(0, end));
    if (source == nullptr)
      return head;
    text.remove_prefix(end);
    cpp::string_view rest = trimmed(text);
    if (!rest.empty() && rest.front() == '[') {
      size_t close = 1;
      while (close < rest.size() && rest[close] != ']')
        ++close;
      parse_actions(rest.substr(1, close - 1), *source);
      text = rest.substr(close < rest.size() ? close + 1 : close);
    }
    *tail = source;
    tail = &source->next;
  }
}

bool read_file(const char *path, char *&text, size_t &size) {
  auto fd = linux_syscalls::open(path, O_RDONLY | O_CLOEXEC, 0);
  if (!fd.has_value())
    return false;
  auto length = linux_syscalls::lseek(fd.value(), 0, SEEK_END);
  bool ok = false;
  if (length.has_value() && length.value() >= 0 &&
      static_cast<size_t>(length.value()) <= MAX_CONFIG_SIZE &&
      linux_syscalls::lseek(fd.value(), 0, SEEK_SET).has_value()) {
    size = static_cast<size_t>(length.value());
    text = static_cast<char *>(::malloc(size + 1));
    if (text != nullptr) {
      size_t done = 0;
      while (done < size) {
        auto got = linux_syscalls::read(fd.value(), text + done, size - done);
        if (!got.has_value() || got.value() <= 0)
          break;
        done += static_cast<size_t>(got.value());
      }
      ok = true;
      size = done;
    }
  }
  linux_syscalls::close(fd.value());
  return ok;
}

// Called with the lock held.
void reload() {
  struct stat st;
  bool present =
      linux_syscalls::newfstatat(AT_FDCWD, config_path, &st, 0).has_value();
  if (current != nullptr) {
    if (!present && !have_stamp)
      return;
    if (present && have_stamp && stamp_inode == static_cast<long>(st.st_ino) &&
        stamp_seconds == static_cast<long>(st.st_mtim.tv_sec) &&
        stamp_nanoseconds == static_cast<long>(st.st_mtim.tv_nsec) &&
        stamp_size == static_cast<long>(st.st_size))
      return;
  }
  char *text = nullptr;
  size_t size = 0;
  Configuration *fresh = nullptr;
  if (present && read_file(config_path, text, size)) {
    fresh = parse(cpp::string_view(text, size));
    ::free(text);
    have_stamp = true;
    stamp_inode = static_cast<long>(st.st_ino);
    stamp_seconds = static_cast<long>(st.st_mtim.tv_sec);
    stamp_nanoseconds = static_cast<long>(st.st_mtim.tv_nsec);
    stamp_size = static_cast<long>(st.st_size);
  } else {
    fresh = parse(cpp::string_view());
    have_stamp = false;
  }
  // The old lists stay where they are: a caller may be walking one.
  if (fresh != nullptr)
    current = fresh;
}

} // namespace

const char *database_name(Database db) {
  return db >= 0 && db < DATABASE_COUNT ? DATABASES[db].name : nullptr;
}

bool database_from_name(cpp::string_view name, Database &db) {
  for (int i = 0; i < DATABASE_COUNT; ++i)
    if (equal_ignoring_case(name, DATABASES[i].name)) {
      db = static_cast<Database>(i);
      return true;
    }
  return false;
}

const char *default_services(Database db) {
  return db >= 0 && db < DATABASE_COUNT ? DATABASES[db].services : "";
}

Configuration *parse(cpp::string_view text) {
  auto *config = static_cast<Configuration *>(::malloc(sizeof(Configuration)));
  if (config == nullptr)
    return nullptr;
  for (int i = 0; i < DATABASE_COUNT; ++i)
    config->sources[i] = nullptr;
  bool seen[DATABASE_COUNT] = {};

  size_t at = 0;
  while (at < text.size()) {
    // A line, with the ones a backslash ends joined to the next.
    size_t end = at;
    while (end < text.size() && text[end] != '\n')
      ++end;
    size_t next = end < text.size() ? end + 1 : end;
    cpp::string_view line = text.substr(at, end - at);
    char *joined = nullptr;
    while (!line.empty() && line.back() == '\\' && next < text.size()) {
      size_t more_end = next;
      while (more_end < text.size() && text[more_end] != '\n')
        ++more_end;
      size_t length = line.size() - 1;
      size_t extra = more_end - next;
      char *buffer = static_cast<char *>(::malloc(length + extra + 2));
      if (buffer == nullptr)
        break;
      size_t n = 0;
      for (size_t i = 0; i < length; ++i)
        buffer[n++] = line[i];
      buffer[n++] = ' ';
      for (size_t i = 0; i < extra; ++i)
        buffer[n++] = text[next + i];
      ::free(joined);
      joined = buffer;
      line = cpp::string_view(joined, n);
      next = more_end < text.size() ? more_end + 1 : more_end;
    }
    size_t hash = 0;
    while (hash < line.size() && line[hash] != '#')
      ++hash;
    line = trimmed(line.substr(0, hash));
    size_t colon = 0;
    while (colon < line.size() && line[colon] != ':')
      ++colon;
    Database db;
    if (colon < line.size() &&
        database_from_name(trimmed(line.substr(0, colon)), db)) {
      // The last line for a database is the one in force.
      config->sources[db] = parse_services(line.substr(colon + 1));
      seen[db] = true;
    }
    ::free(joined);
    at = next;
  }
  for (int i = 0; i < DATABASE_COUNT; ++i)
    if (!seen[i])
      config->sources[i] = parse_services(DATABASES[i].services);
  return config;
}

const Source *sources(Database db) {
  if (db < 0 || db >= DATABASE_COUNT)
    return nullptr;
  lock.lock(cpp::nullopt, false);
  reload();
  const Source *list = overrides[db] != nullptr ? overrides[db]
                       : current != nullptr     ? current->sources[db]
                                                : nullptr;
  lock.unlock();
  return list;
}

bool configure(cpp::string_view database, cpp::string_view services) {
  Database db;
  if (!database_from_name(trimmed(database), db))
    return false;
  Source *list = parse_services(services);
  lock.lock(cpp::nullopt, false);
  overrides[db] = list;
  lock.unlock();
  return true;
}

void TESTONLY_set_config_path(const char *path) {
  lock.lock(cpp::nullopt, false);
  config_path = path;
  current = nullptr;
  have_stamp = false;
  for (auto &o : overrides)
    o = nullptr;
  lock.unlock();
}

void TESTONLY_reset_config_path() { TESTONLY_set_config_path(DEFAULT_PATH); }

} // namespace nss
} // namespace LIBC_NAMESPACE_DECL
