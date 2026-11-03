//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the reading of the netgroup file.
///
//===----------------------------------------------------------------------===//

#include "src/netdb/netgroup_utils.h"

#include "hdr/fcntl_macros.h"
#include "hdr/func/free.h"
#include "hdr/func/malloc.h"
#include "hdr/stdint_proxy.h"
#include "hdr/stdio_macros.h"
#include "src/__support/CPP/string_view.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/close.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/lseek.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/open.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/read.h"
#include "src/__support/macros/attributes.h"
#include "src/__support/macros/config.h"
#include "src/__support/nss/config.h"
#include "src/string/memory_utils/inline_memcpy.h"

namespace LIBC_NAMESPACE_DECL {
namespace netgroup {

namespace {

constexpr size_t NONE = static_cast<size_t>(-1);
constexpr size_t MAX_FILE_SIZE = size_t(1) << 24;
constexpr size_t MAX_TRIPLES = size_t(1) << 16;
// How many groups may name one another before the rest is left out.
constexpr int MAX_DEPTH = 16;

const char *netgroup_path = "/etc/netgroup";

bool is_blank(char c) { return c == ' ' || c == '\t' || c == '\r'; }

// Reads the logical lines of the file: a backslash before a newline joins the
// next line on, and a # starts a comment.
struct Scanner {
  const char *p;
  const char *end;

  void skip_blanks() {
    for (;;) {
      if (p < end && is_blank(*p)) {
        ++p;
      } else if (p + 1 < end && p[0] == '\\' && p[1] == '\n') {
        p += 2;
      } else if (p + 2 < end && p[0] == '\\' && p[1] == '\r' && p[2] == '\n') {
        p += 3;
      } else {
        return;
      }
    }
  }

  void skip_comment() {
    if (p < end && *p == '#')
      while (p < end && *p != '\n')
        ++p;
  }

  // At the end of the logical line, which is the newline or the end of the
  // text.
  bool at_end() {
    skip_blanks();
    skip_comment();
    return p >= end || *p == '\n';
  }

  cpp::string_view word() {
    skip_blanks();
    const char *start = p;
    while (p < end && !is_blank(*p) && *p != '\n' && *p != '(' && *p != '#' &&
           !(p[0] == '\\' && p + 1 < end && p[1] == '\n'))
      ++p;
    return cpp::string_view(start, static_cast<size_t>(p - start));
  }

  void next_line() {
    while (p < end && *p != '\n')
      ++p;
    if (p < end)
      ++p;
  }
};

bool read_file(const char *path, char *&text, size_t &size) {
  auto fd = linux_syscalls::open(path, O_RDONLY | O_CLOEXEC, 0);
  if (!fd.has_value())
    return false;
  auto length = linux_syscalls::lseek(fd.value(), 0, SEEK_END);
  bool ok = false;
  if (length.has_value() && length.value() >= 0 &&
      static_cast<size_t>(length.value()) <= MAX_FILE_SIZE &&
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
      size = done;
      ok = true;
    }
  }
  linux_syscalls::close(fd.value());
  return ok;
}

size_t add_text(Group &group, cpp::string_view text) {
  if (group.text_used + text.size() + 1 > group.text_capacity) {
    size_t capacity = group.text_capacity == 0 ? 256 : group.text_capacity;
    while (group.text_used + text.size() + 1 > capacity)
      capacity *= 2;
    char *grown = static_cast<char *>(::malloc(capacity));
    if (grown == nullptr)
      return NONE - 1;
    if (group.text != nullptr) {
      inline_memcpy(grown, group.text, group.text_used);
      ::free(group.text);
    }
    group.text = grown;
    group.text_capacity = capacity;
  }
  size_t at = group.text_used;
  inline_memcpy(group.text + at, text.data(), text.size());
  group.text[at + text.size()] = '\0';
  group.text_used += text.size() + 1;
  return at;
}

bool add_triple(Group &group, size_t host, size_t user, size_t domain) {
  if (group.count >= MAX_TRIPLES || host == NONE - 1 || user == NONE - 1 ||
      domain == NONE - 1)
    return false;
  if (group.count == group.capacity) {
    size_t capacity = group.capacity == 0 ? 16 : group.capacity * 2;
    auto *grown =
        static_cast<size_t *>(::malloc(capacity * 3 * sizeof(size_t)));
    if (grown == nullptr)
      return false;
    if (group.offsets != nullptr) {
      inline_memcpy(grown, group.offsets, group.count * 3 * sizeof(size_t));
      ::free(group.offsets);
    }
    group.offsets = grown;
    group.capacity = capacity;
  }
  size_t *slot = group.offsets + group.count * 3;
  slot[0] = host;
  slot[1] = user;
  slot[2] = domain;
  ++group.count;
  return true;
}

cpp::string_view trim(cpp::string_view text) {
  while (!text.empty() && is_blank(text.front()))
    text.remove_prefix(1);
  while (!text.empty() && is_blank(text.back()))
    text.remove_suffix(1);
  return text;
}

// An empty field is a wildcard.
size_t add_field(Group &group, cpp::string_view field) {
  field = trim(field);
  return field.empty() ? NONE : add_text(group, field);
}

// The inside of a pair of parentheses: host, user and domain.
bool add_parenthesised(Group &group, cpp::string_view body) {
  size_t comma = body.find_first_of(',');
  cpp::string_view host = body.substr(0, comma);
  cpp::string_view rest = comma == cpp::string_view::npos
                              ? cpp::string_view()
                              : body.substr(comma + 1);
  comma = rest.find_first_of(',');
  cpp::string_view user = rest.substr(0, comma);
  cpp::string_view domain = comma == cpp::string_view::npos
                                ? cpp::string_view()
                                : rest.substr(comma + 1);
  size_t h = add_field(group, host);
  size_t u = add_field(group, user);
  size_t d = add_field(group, domain);
  return add_triple(group, h, u, d);
}

// Whether the text has a line for the group, expanding it into `group`.
bool expand(cpp::string_view file, cpp::string_view name, int depth,
            Group &group) {
  Scanner scan{file.data(), file.data() + file.size()};
  while (scan.p < scan.end) {
    if (scan.at_end()) {
      scan.next_line();
      continue;
    }
    if (scan.word() != name) {
      scan.next_line();
      continue;
    }
    while (!scan.at_end()) {
      scan.skip_blanks();
      if (*scan.p == '(') {
        const char *open = ++scan.p;
        while (scan.p < scan.end && *scan.p != ')' && *scan.p != '\n')
          ++scan.p;
        if (scan.p >= scan.end || *scan.p != ')')
          return true; // A triple with no end is the end of the line.
        add_parenthesised(
            group, cpp::string_view(open, static_cast<size_t>(scan.p - open)));
        ++scan.p;
      } else {
        cpp::string_view inner = scan.word();
        if (inner.empty())
          break;
        if (depth < MAX_DEPTH)
          expand(file, inner, depth + 1, group);
      }
    }
    return true;
  }
  return false;
}

} // anonymous namespace

bool load(cpp::string_view name, Group &group) {
  group = Group{};
  for (const nss::Source *source = nss::sources(nss::NETGROUP);
       source != nullptr; source = source->next) {
    if (cpp::string_view(source->name) != "files")
      continue;
    char *text = nullptr;
    size_t size = 0;
    if (!read_file(netgroup_path, text, size))
      continue;
    bool found = expand(cpp::string_view(text, size), name, 0, group);
    ::free(text);
    if (found)
      return true;
    release(group);
  }
  return false;
}

bool next(Group &group, Triple &triple) {
  if (group.cursor >= group.count)
    return false;
  const size_t *slot = group.offsets + group.cursor * 3;
  triple.host = slot[0] == NONE ? nullptr : group.text + slot[0];
  triple.user = slot[1] == NONE ? nullptr : group.text + slot[1];
  triple.domain = slot[2] == NONE ? nullptr : group.text + slot[2];
  ++group.cursor;
  return true;
}

void release(Group &group) {
  ::free(group.text);
  ::free(group.offsets);
  group = Group{};
}

Group &current() {
  static LIBC_THREAD_LOCAL Group group;
  return group;
}

namespace {
// A field of the netgroup matches when it is absent, or when the argument is,
// or when they are the same.
bool field_matches(const char *field, const char *argument) {
  return field == nullptr || argument == nullptr ||
         cpp::string_view(field) == cpp::string_view(argument);
}
} // anonymous namespace

bool contains(const Group &group, const char *host, const char *user,
              const char *domain) {
  for (size_t i = 0; i < group.count; ++i) {
    const size_t *slot = group.offsets + i * 3;
    const char *h = slot[0] == NONE ? nullptr : group.text + slot[0];
    const char *u = slot[1] == NONE ? nullptr : group.text + slot[1];
    const char *d = slot[2] == NONE ? nullptr : group.text + slot[2];
    if (field_matches(h, host) && field_matches(u, user) &&
        field_matches(d, domain))
      return true;
  }
  return false;
}

void TESTONLY_set_netgroup_path(const char *path) { netgroup_path = path; }
void TESTONLY_reset_netgroup_path() { netgroup_path = "/etc/netgroup"; }

} // namespace netgroup
} // namespace LIBC_NAMESPACE_DECL
