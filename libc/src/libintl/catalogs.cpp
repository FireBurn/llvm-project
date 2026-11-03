//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the registry of text domains and the lookup of translations.
///
//===----------------------------------------------------------------------===//

#include "src/libintl/catalogs.h"

#include "hdr/errno_macros.h"
#include "hdr/fcntl_macros.h"
#include "hdr/func/free.h"
#include "hdr/func/malloc.h"
#include "hdr/locale_macros.h"
#include "hdr/stdint_proxy.h"
#include "hdr/stdio_macros.h"
#include "hdr/sys_mman_macros.h"
#include "src/__support/CPP/string_view.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/close.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/lseek.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/mmap.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/open.h"
#include "src/__support/libc_errno.h"
#include "src/__support/libintl/catalog.h"
#include "src/__support/macros/config.h"
#include "src/__support/threads/raw_mutex.h"
#include "src/iconv/conversion.h"
#include "src/iconv/translit.h"
#include "src/locale/locale.h"

namespace LIBC_NAMESPACE_DECL {
namespace libintl {

namespace {

constexpr const char DEFAULT_DOMAIN[] = "messages";
constexpr const char DEFAULT_DIRECTORY[] = "/usr/share/locale";
constexpr size_t MAX_PATH = 4096;
constexpr size_t MAX_LOCALE_NAME = 128;
// A catalog larger than this is not read.
constexpr size_t MAX_CATALOG_SIZE = size_t(256) << 20;
constexpr size_t CONVERTED_BUCKETS = 64;

struct Binding {
  char *domain;
  char *directory;
  char *codeset;
  Binding *next;
};

// A translation which had to be converted to another character set, kept so
// the same pointer is handed out each time.
struct Converted {
  const char *original;
  char *target;
  char *text;
  Converted *next;
};

// A catalog file looked for once. A missing one is kept as well, so the file
// system is not asked again.
struct Loaded {
  char *key;
  bool present;
  MessageCatalog catalog;
  Converted *converted[CONVERTED_BUCKETS];
  Loaded *next;
};

// A locale.alias file, read once: the pairs of an alias and the locale name it
// stands for.
struct Alias {
  char *name;
  char *value;
  Alias *next;
};

struct AliasFile {
  char *key;
  Alias *aliases;
  AliasFile *next;
};

RawMutex lock;
AliasFile *alias_files = nullptr;
char *current = nullptr;
Binding *bindings = nullptr;
Loaded *loaded = nullptr;

struct Guard {
  LIBC_INLINE Guard() { lock.lock(); }
  LIBC_INLINE ~Guard() { lock.unlock(); }
};

size_t length_of(const char *s) {
  size_t n = 0;
  while (s[n] != '\0')
    ++n;
  return n;
}

char *copy_of(const char *s, size_t n) {
  char *out = static_cast<char *>(::malloc(n + 1));
  if (out == nullptr)
    return nullptr;
  for (size_t i = 0; i < n; ++i)
    out[i] = s[i];
  out[n] = '\0';
  return out;
}

char *copy_of(const char *s) { return copy_of(s, length_of(s)); }

bool same(const char *a, const char *b) {
  return cpp::string_view(a) == cpp::string_view(b);
}

Binding *find_binding(const char *domain) {
  for (Binding *b = bindings; b != nullptr; b = b->next)
    if (same(b->domain, domain))
      return b;
  return nullptr;
}

Binding *binding_for(const char *domain) {
  if (Binding *b = find_binding(domain))
    return b;
  auto *b = static_cast<Binding *>(::malloc(sizeof(Binding)));
  if (b == nullptr)
    return nullptr;
  b->domain = copy_of(domain);
  b->directory = nullptr;
  b->codeset = nullptr;
  if (b->domain == nullptr) {
    ::free(b);
    return nullptr;
  }
  b->next = bindings;
  bindings = b;
  return b;
}

// Replaces a stored string, keeping the old one where there is no memory for
// the new.
const char *store(char *&slot, const char *value) {
  char *fresh = copy_of(value);
  if (fresh == nullptr)
    return nullptr;
  // The old string may still be pointed at by a caller, as in glibc, so it
  // is left where it is.
  slot = fresh;
  return fresh;
}

const char *category_name(int category) {
  switch (category) {
  case LC_CTYPE:
    return "LC_CTYPE";
  case LC_NUMERIC:
    return "LC_NUMERIC";
  case LC_TIME:
    return "LC_TIME";
  case LC_COLLATE:
    return "LC_COLLATE";
  case LC_MONETARY:
    return "LC_MONETARY";
  case LC_MESSAGES:
    return "LC_MESSAGES";
  default:
    return nullptr;
  }
}

// The character set of a catalog as its header names it, which follows
// "charset=" up to the end of the word. The result is empty without one.
cpp::string_view catalog_charset(cpp::string_view header) {
  constexpr cpp::string_view KEY = "charset=";
  for (size_t at = 0; at + KEY.size() <= header.size(); ++at) {
    if (!header.substr(at, KEY.size()).starts_with(KEY))
      continue;
    cpp::string_view rest = header.substr(at + KEY.size());
    size_t end = 0;
    while (end < rest.size() && rest[end] != '\n' && rest[end] != ';' &&
           rest[end] != ' ' && rest[end] != '\t')
      ++end;
    return rest.substr(0, end);
  }
  return cpp::string_view();
}

// The character set a name states, which follows the full stop.
cpp::string_view codeset_in(cpp::string_view locale_name) {
  size_t dot = 0;
  while (dot < locale_name.size() && locale_name[dot] != '.')
    ++dot;
  if (dot == locale_name.size())
    return cpp::string_view();
  cpp::string_view rest = locale_name.substr(dot + 1);
  size_t end = 0;
  while (end < rest.size() && rest[end] != '@')
    ++end;
  return rest.substr(0, end);
}

// A codeset as the directory of a locale writes it: lower case with only the
// letters and digits, and "iso" before a bare number, so "UTF-8" is "utf8"
// and "8859-1" is "iso88591".
size_t normalise_codeset(cpp::string_view codeset, char *out, size_t room) {
  size_t n = 0;
  bool digits_only = !codeset.empty();
  for (char c : codeset) {
    if (c >= '0' && c <= '9')
      continue;
    if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')))
      continue;
    digits_only = false;
  }
  if (digits_only && room > 3) {
    out[n++] = 'i';
    out[n++] = 's';
    out[n++] = 'o';
  }
  for (char c : codeset) {
    char lower = (c >= 'A' && c <= 'Z') ? static_cast<char>(c - 'A' + 'a') : c;
    bool alnum =
        (lower >= 'a' && lower <= 'z') || (lower >= '0' && lower <= '9');
    if (alnum && n + 1 < room)
      out[n++] = lower;
  }
  out[n] = '\0';
  return n;
}

// Maps a catalog file and checks it is a catalog.
bool read_file(const char *path, Loaded &entry) {
  auto fd = linux_syscalls::open(path, O_RDONLY | O_CLOEXEC, 0);
  if (!fd.has_value())
    return false;
  bool ok = false;
  auto size = linux_syscalls::lseek(fd.value(), 0, SEEK_END);
  if (size.has_value() && size.value() > 0 &&
      static_cast<size_t>(size.value()) <= MAX_CATALOG_SIZE) {
    auto map = linux_syscalls::mmap(nullptr, static_cast<size_t>(size.value()),
                                    PROT_READ, MAP_PRIVATE, fd.value(), 0);
    if (map.has_value())
      ok = entry.catalog.load(map.value(), static_cast<size_t>(size.value()));
    // The mapping is kept when it is a catalog, for as long as the process
    // runs, and not worth giving back otherwise.
  }
  linux_syscalls::close(fd.value());
  return ok;
}

Loaded *load(const char *path) {
  for (Loaded *l = loaded; l != nullptr; l = l->next)
    if (same(l->key, path))
      return l;
  auto *entry = static_cast<Loaded *>(::malloc(sizeof(Loaded)));
  if (entry == nullptr)
    return nullptr;
  entry->key = copy_of(path);
  if (entry->key == nullptr) {
    ::free(entry);
    return nullptr;
  }
  entry->catalog = MessageCatalog();
  for (size_t i = 0; i < CONVERTED_BUCKETS; ++i)
    entry->converted[i] = nullptr;
  entry->present = read_file(path, *entry);
  entry->next = loaded;
  loaded = entry;
  return entry;
}

// Appends to a path being built, which has to end up shorter than MAX_PATH.
struct PathBuilder {
  char text[MAX_PATH];
  size_t size = 0;
  bool overflow = false;
  void add(cpp::string_view s) {
    if (size + s.size() >= MAX_PATH) {
      overflow = true;
      return;
    }
    for (char c : s)
      text[size++] = c;
    text[size] = '\0';
  }
};

// Converts a translation between character sets, giving back the text
// unchanged where either set is not known or the conversion cannot be done.
// What cannot be written in the target is transliterated, and where there is
// no transliteration it is a question mark.
const char *convert(Loaded &entry, const char *text, cpp::string_view from_name,
                    const char *to_name) {
  char from[64];
  size_t n = 0;
  for (; n < from_name.size() && n + 1 < sizeof(from); ++n)
    from[n] = from_name[n];
  from[n] = '\0';
  const auto *from_set = iconv_internal::find_charset(from);
  const auto *to_set = iconv_internal::find_charset(to_name);
  if (from_set == nullptr || to_set == nullptr ||
      (from_set->encoding == to_set->encoding &&
       from_set->table == to_set->table))
    return text;

  size_t bucket = (reinterpret_cast<uintptr_t>(text) >> 3) % CONVERTED_BUCKETS;
  for (Converted *c = entry.converted[bucket]; c != nullptr; c = c->next)
    if (c->original == text && same(c->target, to_name))
      return c->text;

  iconv_internal::Conversion conv;
  conv.from = from_set->encoding;
  conv.to = to_set->encoding;
  conv.from_table = from_set->table;
  conv.to_table = to_set->table;
  conv.from_combining = from_set->combining;
  conv.to_combining = to_set->combining;
  conv.from_sequences = from_set->sequences;
  conv.to_sequences = to_set->sequences;
  conv.ignore = false;
  conv.translit = false;
  conv.irreversible = 0;
  iconv_internal::reset_state(conv);
  conv.read_big = !Endian::IS_LITTLE;

  const size_t in_size = length_of(text);
  // Every set writes at most four bytes for a character that takes one byte
  // in some other, and a transliteration can write several characters.
  const size_t room = in_size * 8 + 16;
  auto *out = static_cast<unsigned char *>(::malloc(room));
  if (out == nullptr)
    return text;
  size_t used_out = 0;
  size_t at = 0;
  bool failed = false;
  const auto *in = reinterpret_cast<const unsigned char *>(text);
  while (at < in_size && !failed) {
    char32_t cp = 0;
    size_t used = 0;
    iconv_internal::Status status =
        iconv_internal::decode(conv, in + at, in_size - at, cp, used);
    if (status == iconv_internal::Status::NONE) {
      at += used;
      continue;
    }
    if (status != iconv_internal::Status::OK) {
      failed = true;
      break;
    }
    at += used;
    size_t made = 0;
    status =
        iconv_internal::encode(conv, cp, out + used_out, room - used_out, made);
    if (status == iconv_internal::Status::INVALID) {
      const char16_t *alt = nullptr;
      size_t alt_len = 0;
      made = 0;
      bool written = false;
      if (iconv_internal::find_transliteration(cp, alt, alt_len)) {
        size_t total = 0;
        written = true;
        for (size_t i = 0; i < alt_len && written; ++i) {
          size_t one = 0;
          if (iconv_internal::encode(conv, alt[i], out + used_out + total,
                                     room - used_out - total,
                                     one) != iconv_internal::Status::OK)
            written = false;
          total += one;
        }
        if (written)
          made = total;
      }
      if (!written) {
        made = 0;
        status = iconv_internal::encode(conv, '?', out + used_out,
                                        room - used_out, made);
        if (status != iconv_internal::Status::OK)
          failed = true;
      }
    } else if (status != iconv_internal::Status::OK) {
      failed = true;
    }
    used_out += made;
  }
  if (!failed) {
    size_t made = 0;
    if (iconv_internal::unshift(conv, out + used_out, room - used_out, made) !=
        iconv_internal::Status::OK)
      failed = true;
    used_out += made;
  }
  if (failed || used_out >= room) {
    ::free(out);
    return text;
  }
  out[used_out] = '\0';
  auto *record = static_cast<Converted *>(::malloc(sizeof(Converted)));
  if (record != nullptr)
    record->target = copy_of(to_name);
  if (record == nullptr || record->target == nullptr) {
    ::free(record);
    ::free(out);
    return text;
  }
  record->original = text;
  record->text = reinterpret_cast<char *>(out);
  record->next = entry.converted[bucket];
  entry.converted[bucket] = record;
  return record->text;
}

char lower_of(char c) { return (c >= 'A' && c <= 'Z') ? c - 'A' + 'a' : c; }

bool same_ignoring_case(const char *a, const char *b) {
  for (; *a != '\0' && lower_of(*a) == lower_of(*b); ++a, ++b)
    ;
  return *a == '\0' && *b == '\0';
}

bool is_blank(char c) { return c == ' ' || c == '\t' || c == '\r'; }

// Reads the lines of an alias file, each "alias value" with the two parted by
// blanks and a hash starting a comment line.
void parse_aliases(const char *text, size_t size, AliasFile &file) {
  size_t at = 0;
  while (at < size) {
    size_t end = at;
    while (end < size && text[end] != '\n')
      ++end;
    size_t i = at;
    while (i < end && is_blank(text[i]))
      ++i;
    if (i < end && text[i] != '#') {
      size_t name_end = i;
      while (name_end < end && !is_blank(text[name_end]))
        ++name_end;
      size_t value = name_end;
      while (value < end && is_blank(text[value]))
        ++value;
      size_t value_end = value;
      while (value_end < end && !is_blank(text[value_end]))
        ++value_end;
      if (value_end > value) {
        auto *alias = static_cast<Alias *>(::malloc(sizeof(Alias)));
        if (alias != nullptr) {
          alias->name = copy_of(text + i, name_end - i);
          alias->value = copy_of(text + value, value_end - value);
          if (alias->name == nullptr || alias->value == nullptr) {
            ::free(alias->name);
            ::free(alias->value);
            ::free(alias);
          } else {
            alias->next = file.aliases;
            file.aliases = alias;
          }
        }
      }
    }
    at = end + 1;
  }
}

AliasFile *alias_file(const char *directory) {
  PathBuilder path;
  path.add(directory);
  path.add("/locale.alias");
  if (path.overflow)
    return nullptr;
  for (AliasFile *f = alias_files; f != nullptr; f = f->next)
    if (same(f->key, path.text))
      return f;
  auto *file = static_cast<AliasFile *>(::malloc(sizeof(AliasFile)));
  if (file == nullptr)
    return nullptr;
  file->key = copy_of(path.text);
  file->aliases = nullptr;
  if (file->key == nullptr) {
    ::free(file);
    return nullptr;
  }
  auto fd = linux_syscalls::open(path.text, O_RDONLY | O_CLOEXEC, 0);
  if (fd.has_value()) {
    auto size = linux_syscalls::lseek(fd.value(), 0, SEEK_END);
    if (size.has_value() && size.value() > 0 &&
        static_cast<size_t>(size.value()) <= MAX_CATALOG_SIZE) {
      auto map =
          linux_syscalls::mmap(nullptr, static_cast<size_t>(size.value()),
                               PROT_READ, MAP_PRIVATE, fd.value(), 0);
      if (map.has_value())
        parse_aliases(static_cast<const char *>(map.value()),
                      static_cast<size_t>(size.value()), *file);
    }
    linux_syscalls::close(fd.value());
  }
  file->next = alias_files;
  alias_files = file;
  return file;
}

// The locale name an alias such as "german" stands for, or nullptr where the
// name is not an alias. An alias may stand for another, a few times over.
const char *expand_alias(const char *directory, const char *name) {
  AliasFile *file = alias_file(directory);
  if (file == nullptr)
    return nullptr;
  const char *name_now = name;
  for (int depth = 0; depth < 5; ++depth) {
    const char *next = nullptr;
    for (Alias *a = file->aliases; a != nullptr; a = a->next)
      if (same_ignoring_case(a->name, name_now)) {
        next = a->value;
        break;
      }
    if (next == nullptr)
      break;
    name_now = next;
  }
  return name_now == name ? nullptr : name_now;
}

// The pieces of a locale name: language, territory, codeset and modifier.
struct LocaleParts {
  cpp::string_view language;
  cpp::string_view territory;
  cpp::string_view codeset;
  cpp::string_view modifier;
};

LocaleParts split_name(cpp::string_view name) {
  LocaleParts parts;
  size_t i = 0;
  size_t start = 0;
  auto until = [&](bool stop_underscore, bool stop_dot) {
    start = i;
    while (i < name.size() && name[i] != '@' &&
           !(stop_underscore && name[i] == '_') &&
           !(stop_dot && name[i] == '.'))
      ++i;
    return name.substr(start, i - start);
  };
  parts.language = until(true, true);
  if (i < name.size() && name[i] == '_') {
    ++i;
    parts.territory = until(false, true);
  }
  if (i < name.size() && name[i] == '.') {
    ++i;
    parts.codeset = until(false, false);
  }
  if (i < name.size() && name[i] == '@') {
    ++i;
    parts.modifier = name.substr(i);
  }
  return parts;
}

// Looks for the catalog of a domain under one locale name, trying the forms
// of the name from the fullest to the bare language as gettext does.
bool find_in(const char *directory, cpp::string_view locale,
             const char *category, const char *domain, Loaded *&found) {
  LocaleParts parts = split_name(locale);
  if (parts.language.empty())
    return false;
  char normal[64];
  normalise_codeset(parts.codeset, normal, sizeof(normal));

  // Bit 4 is the modifier, 2 the territory and 1 the codeset.
  for (int mask = 7; mask >= 0; --mask) {
    if ((mask & 4) && parts.modifier.empty())
      continue;
    if ((mask & 2) && parts.territory.empty())
      continue;
    if ((mask & 1) && parts.codeset.empty())
      continue;
    const int variants = (mask & 1) && !same(normal, "") ? 2 : 1;
    for (int variant = 0; variant < variants; ++variant) {
      PathBuilder path;
      path.add(directory);
      path.add("/");
      path.add(parts.language);
      if (mask & 2) {
        path.add("_");
        path.add(parts.territory);
      }
      if (mask & 1) {
        path.add(".");
        if (variant == 0)
          path.add(normal);
        else
          path.add(parts.codeset);
      }
      if (mask & 4) {
        path.add("@");
        path.add(parts.modifier);
      }
      path.add("/");
      path.add(category);
      path.add("/");
      path.add(domain);
      path.add(".mo");
      if (path.overflow)
        continue;
      Loaded *entry = load(path.text);
      if (entry != nullptr && entry->present) {
        found = entry;
        return true;
      }
    }
  }
  return false;
}

// Looks for the catalog under a locale name, and then under the name it is an
// alias for. |used| is the name the catalog was found under.
bool find_named(const char *directory, cpp::string_view name,
                const char *category, const char *domain, Loaded *&found,
                cpp::string_view &used) {
  if (find_in(directory, name, category, domain, found)) {
    used = name;
    return true;
  }
  char buffer[MAX_LOCALE_NAME];
  size_t n = 0;
  for (; n < name.size() && n + 1 < sizeof(buffer); ++n)
    buffer[n] = name[n];
  buffer[n] = '\0';
  const char *alias = expand_alias(directory, buffer);
  if (alias != nullptr && find_in(directory, alias, category, domain, found)) {
    used = alias;
    return true;
  }
  return false;
}

} // namespace

const char *current_domain() {
  Guard guard;
  return current != nullptr ? current : DEFAULT_DOMAIN;
}

const char *set_current_domain(const char *name) {
  Guard guard;
  if (name[0] == '\0')
    name = DEFAULT_DOMAIN;
  if (current != nullptr && same(current, name))
    return current;
  return store(current, name);
}

const char *domain_directory(const char *domain) {
  Guard guard;
  Binding *b = find_binding(domain);
  return b != nullptr && b->directory != nullptr ? b->directory
                                                 : DEFAULT_DIRECTORY;
}

const char *set_domain_directory(const char *domain, const char *directory) {
  Guard guard;
  Binding *b = binding_for(domain);
  if (b == nullptr)
    return nullptr;
  if (b->directory != nullptr && same(b->directory, directory))
    return b->directory;
  return store(b->directory, directory);
}

const char *domain_codeset(const char *domain) {
  Guard guard;
  Binding *b = find_binding(domain);
  return b != nullptr ? b->codeset : nullptr;
}

const char *set_domain_codeset(const char *domain, const char *codeset) {
  Guard guard;
  Binding *b = binding_for(domain);
  if (b == nullptr)
    return nullptr;
  if (b->codeset != nullptr && same(b->codeset, codeset))
    return b->codeset;
  return store(b->codeset, codeset);
}

const char *translate(const char *domain, const char *msgid, bool plural,
                      unsigned long count, int category,
                      const char *locale_name) {
  const char *category_dir = category_name(category);
  if (category_dir == nullptr || locale_name == nullptr)
    return nullptr;
  cpp::string_view locale = locale_name;
  if (locale.empty() || locale == "C" || locale == "POSIX")
    return nullptr;

  // The languages asked for, the first of which that has a catalog is used.
  // LANGUAGE puts its list ahead of the locale, as it does in glibc.
  const char *language = internal::environment_value("LANGUAGE");

  const int saved_errno = libc_errno;
  const char *result = nullptr;
  {
    Guard guard;
    Binding *b = find_binding(domain);
    const char *directory = b != nullptr && b->directory != nullptr
                                ? b->directory
                                : DEFAULT_DIRECTORY;
    const char *asked_codeset = b != nullptr ? b->codeset : nullptr;

    Loaded *found = nullptr;
    cpp::string_view chosen;
    bool stop = false;
    if (language != nullptr && language[0] != '\0') {
      cpp::string_view list = language;
      size_t start = 0;
      while (start <= list.size() && found == nullptr && !stop) {
        size_t end = start;
        while (end < list.size() && list[end] != ':')
          ++end;
        cpp::string_view entry = list.substr(start, end - start);
        if (entry == "C" || entry == "POSIX") {
          stop = true;
        } else if (!entry.empty()) {
          find_named(directory, entry, category_dir, domain, found, chosen);
        }
        start = end + 1;
      }
    }
    if (found == nullptr && !stop)
      find_named(directory, locale, category_dir, domain, found, chosen);

    if (found != nullptr) {
      const char *text = found->catalog.translate(msgid, plural, count);
      if (text != nullptr) {
        cpp::string_view target = asked_codeset != nullptr
                                      ? cpp::string_view(asked_codeset)
                                      : codeset_in(chosen);
        char target_name[64];
        size_t n = 0;
        if (target.empty()) {
          const char *current_set = internal::current_codeset();
          for (; current_set[n] != '\0' && n + 1 < sizeof(target_name); ++n)
            target_name[n] = current_set[n];
        } else {
          for (; n < target.size() && n + 1 < sizeof(target_name); ++n)
            target_name[n] = target[n];
        }
        target_name[n] = '\0';
        result = convert(*found, text, catalog_charset(found->catalog.header()),
                         target_name);
      }
    }
  }
  libc_errno = saved_errno;
  return result;
}

const char *lookup(const char *domain, const char *msgid1, const char *msgid2,
                   unsigned long count, int category, const char *locale_name) {
  if (msgid1 == nullptr)
    return nullptr;
  const bool plural = msgid2 != nullptr;
  const char *fallback = plural && count != 1 ? msgid2 : msgid1;
  if (domain == nullptr)
    domain = current_domain();
  const char *text =
      translate(domain, msgid1, plural, count, category, locale_name);
  return text != nullptr ? text : fallback;
}

} // namespace libintl
} // namespace LIBC_NAMESPACE_DECL
