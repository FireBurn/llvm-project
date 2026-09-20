//===-- Implementation of wordexp -----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// What this does and does not do
// ------------------------------
// Tilde expansion, parameter expansion, field splitting, quote removal and
// pathname expansion, in that order.
//
// Command substitution is not performed. `$(...)` and `` `...` `` are
// answered with WRDE_CMDSUB whether or not the caller passed WRDE_NOCMD,
// and arithmetic expansion, which opens the same way, goes with them. A
// caller that wants a command run has to run it; nothing here will. That
// keeps the whole of this file free of any path that executes anything,
// which is not true of the two usual implementations: one hands the words
// to /bin/sh, and the other has had the refusal itself go wrong.
//
// Parameter expansion is `$NAME` and `${NAME}`. The forms which carry a
// word of their own, `${NAME:-word}` and the rest, are answered with
// WRDE_SYNTAX rather than quietly coming out wrong.
//
//===----------------------------------------------------------------------===//

#include "src/wordexp/wordexp.h"
#include "hdr/func/free.h"
#include "hdr/func/malloc.h"
#include "hdr/func/realloc.h"
#include "hdr/glob_macros.h"
#include "hdr/types/glob_t.h"
#include "hdr/types/wordexp_t.h"
#include "hdr/wordexp_macros.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/glob/glob.h"
#include "src/glob/globfree.h"
#include "src/pwd/getpwnam.h"
#include "src/stdlib/getenv.h"
#include "hdr/types/struct_passwd.h"

namespace LIBC_NAMESPACE_DECL {

namespace {

// The default IFS. Nothing here reads the caller's, since a program has no
// way to set one that this would see.
bool is_ifs(char c) { return c == ' ' || c == '\t' || c == '\n'; }

// A character which may only appear quoted, because unquoted it would mean
// something to a shell that this does not act on.
bool is_special(char c) {
  switch (c) {
  case '|':
  case '&':
  case ';':
  case '<':
  case '>':
  case '(':
  case ')':
  case '{':
  case '}':
  case '\n':
    return true;
  default:
    return false;
  }
}

bool is_name_start(char c) {
  return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_';
}

bool is_name_char(char c) { return is_name_start(c) || (c >= '0' && c <= '9'); }

// A string built up a piece at a time.
class Text {
  char *buf = nullptr;
  size_t len = 0;
  size_t cap = 0;

public:
  ~Text() { free(buf); }

  bool reserve(size_t want) {
    if (want <= cap)
      return true;
    size_t next = cap ? cap * 2 : 64;
    while (next < want)
      next *= 2;
    char *grown = reinterpret_cast<char *>(realloc(buf, next));
    if (grown == nullptr)
      return false;
    buf = grown;
    cap = next;
    return true;
  }

  bool push(char c) {
    if (!reserve(len + 1))
      return false;
    buf[len++] = c;
    return true;
  }

  bool append(const char *s) {
    for (; *s != '\0'; ++s)
      if (!push(*s))
        return false;
    return true;
  }

  void clear() { len = 0; }
  size_t size() const { return len; }

  // The text so far, terminated. It stays owned here.
  const char *c_str() {
    if (!reserve(len + 1))
      return nullptr;
    buf[len] = '\0';
    return buf;
  }
};

// The words as they accumulate, kept in the shape wordexp_t wants.
class Words {
  wordexp_t *we;
  size_t cap = 0;

public:
  explicit Words(wordexp_t *w) : we(w) {}

  // Room for one more word and the null which follows the last.
  bool reserve_one() {
    size_t used = we->we_offs + we->we_wordc;
    if (used + 2 <= cap)
      return true;
    size_t next = cap ? cap * 2 : 16;
    while (used + 2 > next)
      next *= 2;
    char **grown =
        reinterpret_cast<char **>(realloc(we->we_wordv, next * sizeof(char *)));
    if (grown == nullptr)
      return false;
    we->we_wordv = grown;
    cap = next;
    return true;
  }

  // Takes a copy of `word`.
  bool add(const char *word) {
    if (!reserve_one())
      return false;
    size_t n = 0;
    while (word[n] != '\0')
      ++n;
    ++n;
    char *copy = reinterpret_cast<char *>(malloc(n));
    if (copy == nullptr)
      return false;
    for (size_t i = 0; i < n; ++i)
      copy[i] = word[i];
    we->we_wordv[we->we_offs + we->we_wordc] = copy;
    ++we->we_wordc;
    we->we_wordv[we->we_offs + we->we_wordc] = nullptr;
    return true;
  }

  // Sets up the empty slots at the front and the terminating null, for a
  // vector which is being started rather than added to.
  bool start() {
    if (!reserve_one())
      return false;
    for (size_t i = 0; i < we->we_offs; ++i)
      we->we_wordv[i] = nullptr;
    we->we_wordv[we->we_offs] = nullptr;
    return true;
  }
};

// Everything the caller's directory would match, or the word itself when it
// matches nothing, which is what a shell does when nullglob is off.
int add_globbed(Words &words, const char *word, bool has_magic) {
  if (!has_magic)
    return words.add(word) ? 0 : WRDE_NOSPACE;

  glob_t g;
  for (size_t i = 0; i < sizeof(glob_t); ++i)
    reinterpret_cast<char *>(&g)[i] = 0;

  int rc = LIBC_NAMESPACE::glob(word, 0, nullptr, &g);
  if (rc == GLOB_NOSPACE)
    return WRDE_NOSPACE;
  if (rc != 0 || g.gl_pathc == 0) {
    // No match, so the word stands as it is.
    LIBC_NAMESPACE::globfree(&g);
    return words.add(word) ? 0 : WRDE_NOSPACE;
  }

  for (size_t i = 0; i < g.gl_pathc; ++i) {
    if (!words.add(g.gl_pathv[g.gl_offs + i])) {
      LIBC_NAMESPACE::globfree(&g);
      return WRDE_NOSPACE;
    }
  }
  LIBC_NAMESPACE::globfree(&g);
  return 0;
}

} // anonymous namespace

LLVM_LIBC_FUNCTION(int, wordexp,
                   (const char *__restrict words, wordexp_t *__restrict we,
                    int flags)) {
  if (words == nullptr || we == nullptr)
    return WRDE_SYNTAX;

  if ((flags & WRDE_REUSE) != 0) {
    // The caller says this came from an earlier call, so let go of that
    // before anything else.
    if (we->we_wordv != nullptr) {
      char **w = we->we_wordv + we->we_offs;
      for (; *w != nullptr; ++w)
        free(*w);
      free(we->we_wordv);
    }
    we->we_wordv = nullptr;
    we->we_wordc = 0;
  }

  if ((flags & WRDE_DOOFFS) == 0)
    we->we_offs = 0;

  const bool appending = (flags & WRDE_APPEND) != 0 && we->we_wordv != nullptr;
  if (!appending) {
    we->we_wordv = nullptr;
    we->we_wordc = 0;
  }

  Words out(we);
  if (!appending && !out.start()) {
    return WRDE_NOSPACE;
  }

  Text field;
  bool field_open = false; // Something was written, even if it came to "".
  bool has_magic = false;  // The field holds an unquoted glob character.

  auto flush = [&](void) -> int {
    if (!field_open)
      return 0;
    const char *s = field.c_str();
    if (s == nullptr)
      return WRDE_NOSPACE;
    int rc = add_globbed(out, s, has_magic);
    field.clear();
    field_open = false;
    has_magic = false;
    return rc;
  };

  // Expands the name at `p` and writes it into the field. `p` is left on
  // the last character of the name.
  auto expand_name = [&](const char *&p) -> int {
    Text name;
    if (*p == '{') {
      ++p;
      for (; *p != '\0' && *p != '}'; ++p) {
        // The forms which carry a word of their own are not done here.
        if (!is_name_char(*p))
          return WRDE_SYNTAX;
        if (!name.push(*p))
          return WRDE_NOSPACE;
      }
      if (*p != '}')
        return WRDE_SYNTAX;
    } else {
      if (!is_name_start(*p))
        return WRDE_SYNTAX;
      for (; is_name_char(*(p + 1)); ++p)
        if (!name.push(*p))
          return WRDE_NOSPACE;
      if (!name.push(*p))
        return WRDE_NOSPACE;
    }

    const char *key = name.c_str();
    if (key == nullptr)
      return WRDE_NOSPACE;
    const char *value = LIBC_NAMESPACE::getenv(key);
    if (value == nullptr) {
      if ((flags & WRDE_UNDEF) != 0)
        return WRDE_BADVAL;
      return 0; // An unset variable comes to nothing.
    }
    // What a variable expands to is subject to field splitting.
    for (const char *v = value; *v != '\0'; ++v) {
      if (is_ifs(*v)) {
        int rc = flush();
        if (rc != 0)
          return rc;
        continue;
      }
      field_open = true;
      if (!field.push(*v))
        return WRDE_NOSPACE;
    }
    return 0;
  };

  for (const char *p = words; *p != '\0'; ++p) {
    char c = *p;

    // Command substitution, and the arithmetic which opens the same way.
    if (c == '`')
      return WRDE_CMDSUB;
    if (c == '$' && *(p + 1) == '(')
      return WRDE_CMDSUB;

    if (c == '\\') {
      if (*(p + 1) == '\0')
        return WRDE_SYNTAX;
      ++p;
      field_open = true;
      if (!field.push(*p))
        return WRDE_NOSPACE;
      continue;
    }

    if (c == '\'') {
      field_open = true;
      ++p;
      for (; *p != '\0' && *p != '\''; ++p)
        if (!field.push(*p))
          return WRDE_NOSPACE;
      if (*p != '\'')
        return WRDE_SYNTAX;
      continue;
    }

    if (c == '"') {
      field_open = true;
      ++p;
      for (; *p != '\0' && *p != '"'; ++p) {
        if (*p == '`')
          return WRDE_CMDSUB;
        if (*p == '$' && *(p + 1) == '(')
          return WRDE_CMDSUB;
        if (*p == '\\') {
          char next = *(p + 1);
          // Inside double quotes a backslash only escapes these.
          if (next == '$' || next == '`' || next == '"' || next == '\\') {
            ++p;
            if (!field.push(*p))
              return WRDE_NOSPACE;
            continue;
          }
          if (!field.push(*p))
            return WRDE_NOSPACE;
          continue;
        }
        if (*p == '$') {
          ++p;
          // No field splitting inside double quotes, so take the value
          // whole rather than going through expand_name.
          Text name;
          bool braced = (*p == '{');
          if (braced)
            ++p;
          if (!braced && !is_name_start(*p))
            return WRDE_SYNTAX;
          for (; *p != '\0'; ++p) {
            if (braced && *p == '}')
              break;
            if (!is_name_char(*p)) {
              if (braced)
                return WRDE_SYNTAX;
              break;
            }
            if (!name.push(*p))
              return WRDE_NOSPACE;
          }
          if (braced && *p != '}')
            return WRDE_SYNTAX;
          if (!braced)
            --p;
          const char *key = name.c_str();
          if (key == nullptr)
            return WRDE_NOSPACE;
          const char *value = LIBC_NAMESPACE::getenv(key);
          if (value == nullptr) {
            if ((flags & WRDE_UNDEF) != 0)
              return WRDE_BADVAL;
            continue;
          }
          if (!field.append(value))
            return WRDE_NOSPACE;
          continue;
        }
        if (!field.push(*p))
          return WRDE_NOSPACE;
      }
      if (*p != '"')
        return WRDE_SYNTAX;
      continue;
    }

    if (c == '$') {
      ++p;
      int rc = expand_name(p);
      if (rc != 0)
        return rc;
      continue;
    }

    if (c == '~' && field.size() == 0) {
      // A tilde stands for a home directory, but only at the front of a
      // field and only up to the first slash.
      Text user;
      const char *q = p + 1;
      for (; *q != '\0' && *q != '/' && !is_ifs(*q); ++q)
        if (!user.push(*q))
          return WRDE_NOSPACE;
      const char *who = user.c_str();
      if (who == nullptr)
        return WRDE_NOSPACE;
      const char *home = nullptr;
      if (*who == '\0') {
        home = LIBC_NAMESPACE::getenv("HOME");
      } else {
        struct passwd *pw = LIBC_NAMESPACE::getpwnam(who);
        if (pw != nullptr)
          home = pw->pw_dir;
      }
      if (home != nullptr) {
        field_open = true;
        if (!field.append(home))
          return WRDE_NOSPACE;
        p = q - 1;
        continue;
      }
      // Nobody of that name, so the tilde is just a character.
    }

    if (is_ifs(c)) {
      int rc = flush();
      if (rc != 0)
        return rc;
      continue;
    }

    if (is_special(c))
      return WRDE_BADCHAR;

    if (c == '*' || c == '?' || c == '[')
      has_magic = true;

    field_open = true;
    if (!field.push(c))
      return WRDE_NOSPACE;
  }

  return flush();
}

} // namespace LIBC_NAMESPACE_DECL
