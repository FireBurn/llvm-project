//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// The pattern matcher behind fnmatch.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_FNMATCH_FNMATCH_MATCHER_H
#define LLVM_LIBC_SRC_FNMATCH_FNMATCH_MATCHER_H

#include "hdr/fnmatch_macros.h"
#include "src/__support/CPP/string_view.h"
#include "src/__support/ctype_utils.h"
#include "src/__support/macros/attributes.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {
namespace fnmatch_internal {

using cpp::string_view;

LIBC_INLINE constexpr bool isblank(char c) { return c == ' ' || c == '\t'; }
LIBC_INLINE constexpr bool iscntrl(char c) {
  return static_cast<unsigned char>(c) < 0x20 ||
         static_cast<unsigned char>(c) == 0x7f;
}
LIBC_INLINE constexpr bool isprint(char c) {
  return static_cast<unsigned char>(c) >= 0x20 &&
         static_cast<unsigned char>(c) < 0x7f;
}
LIBC_INLINE constexpr bool ispunct(char c) {
  return internal::isgraph(c) && !internal::isalnum(c);
}
LIBC_INLINE constexpr bool isxdigit(char c) {
  return internal::isdigit(c) || (c >= 'a' && c <= 'f') ||
         (c >= 'A' && c <= 'F');
}

LIBC_INLINE constexpr bool in_class(string_view name, char c) {
  if (name == "alpha")
    return internal::isalpha(c);
  if (name == "digit")
    return internal::isdigit(c);
  if (name == "alnum")
    return internal::isalnum(c);
  if (name == "upper")
    return internal::isupper(c);
  if (name == "lower")
    return internal::islower(c);
  if (name == "space")
    return internal::isspace(c);
  if (name == "blank")
    return isblank(c);
  if (name == "cntrl")
    return iscntrl(c);
  if (name == "print")
    return isprint(c);
  if (name == "graph")
    return internal::isgraph(c);
  if (name == "punct")
    return ispunct(c);
  if (name == "xdigit")
    return isxdigit(c);
  // An unknown class name matches nothing rather than being an error.
  return false;
}

class Matcher {
  const char *pattern;
  const char *string;
  int flags;

  LIBC_INLINE bool pathname() const { return (flags & FNM_PATHNAME) != 0; }
  LIBC_INLINE bool noescape() const { return (flags & FNM_NOESCAPE) != 0; }
  LIBC_INLINE bool casefold() const { return (flags & FNM_CASEFOLD) != 0; }
  LIBC_INLINE bool period() const { return (flags & FNM_PERIOD) != 0; }

  LIBC_INLINE char fold(char c) const {
    return casefold() ? internal::tolower(c) : c;
  }

  // Whether |s| sits where a leading period must be matched explicitly: the
  // start of the string, or just after a '/' when slashes are significant.
  LIBC_INLINE bool at_period(const char *s) const {
    if (!period() || *s != '.')
      return false;
    if (s == string)
      return true;
    return pathname() && s[-1] == '/';
  }

  // Matches a bracket expression. |p| points just past the '['. On a match
  // |p| is advanced past the ']'; on a syntax error the whole expression is
  // rejected and |p| is left alone so the caller can treat '[' as literal.
  bool match_bracket(const char *&p, char c) const;

public:
  LIBC_INLINE Matcher(const char *pattern_, const char *string_, int flags_)
      : pattern(pattern_), string(string_), flags(flags_) {}

  // Matches |p| against |s|, both of which may run to the end of the
  // pattern and string respectively.
  bool match(const char *p, const char *s) const;

  LIBC_INLINE bool run() const { return match(pattern, string); }
};

LIBC_INLINE bool Matcher::match_bracket(const char *&p, char c) const {
  const char *cursor = p;
  bool negated = false;
  if (*cursor == '!' || *cursor == '^') {
    negated = true;
    ++cursor;
  }

  bool matched = false;
  bool first = true;
  for (;; first = false) {
    if (*cursor == '\0')
      return false; // Unterminated, so '[' was not a bracket after all.
    // A ']' is a literal only when it is the first character in the set.
    if (*cursor == ']' && !first) {
      ++cursor;
      break;
    }

    // A character class, [:name:].
    if (cursor[0] == '[' && cursor[1] == ':') {
      const char *name_start = cursor + 2;
      const char *name_end = name_start;
      while (*name_end != '\0' && *name_end != ':')
        ++name_end;
      if (name_end[0] != ':' || name_end[1] != ']')
        return false;
      string_view name(name_start, static_cast<size_t>(name_end - name_start));
      matched = matched || in_class(name, c);
      cursor = name_end + 2;
      continue;
    }

    char low = *cursor;
    if (low == '\\' && !noescape() && cursor[1] != '\0')
      low = *++cursor;
    ++cursor;

    // A range, but only when the '-' is not the last character in the set.
    if (cursor[0] == '-' && cursor[1] != ']' && cursor[1] != '\0') {
      ++cursor;
      char high = *cursor;
      if (high == '\\' && !noescape() && cursor[1] != '\0')
        high = *++cursor;
      ++cursor;
      char folded = fold(c);
      matched = matched || (fold(low) <= folded && folded <= fold(high));
      continue;
    }

    matched = matched || fold(low) == fold(c);
  }

  // A slash is never matched by a bracket when slashes are significant.
  if (pathname() && c == '/')
    return false;

  p = cursor;
  return matched != negated;
}

LIBC_INLINE bool Matcher::match(const char *p, const char *s) const {
  // The position the last '*' was seen at, so a failed attempt can come back
  // and let it consume one more character. This keeps the common case out of
  // exponential backtracking.
  const char *star_p = nullptr;
  const char *star_s = nullptr;

  for (;;) {
    // FNM_LEADING_DIR lets a pattern which is used up match a prefix of the
    // string, so long as the string carries on with a '/'.
    if (*p == '\0' && (flags & FNM_LEADING_DIR) != 0 && *s == '/')
      return true;

    if (*s != '\0') {
      char pc = *p;
      switch (pc) {
      case '*': {
        // A '*' never matches a leading period which has to be explicit.
        if (at_period(s))
          return false;
        star_p = p++;
        star_s = s;
        continue;
      }
      case '?':
        if (at_period(s))
          break;
        if (pathname() && *s == '/')
          break;
        ++p;
        ++s;
        continue;
      case '[': {
        if (at_period(s))
          break;
        const char *after = p + 1;
        if (match_bracket(after, *s)) {
          p = after;
          ++s;
          continue;
        }
        // A malformed set leaves |after| alone and '[' is literal.
        if (after == p + 1 && fold(pc) == fold(*s)) {
          ++p;
          ++s;
          continue;
        }
        break;
      }
      case '\\':
        if (!noescape() && p[1] != '\0') {
          if (fold(p[1]) == fold(*s)) {
            p += 2;
            ++s;
            continue;
          }
          break;
        }
        [[fallthrough]];
      default:
        if (pc != '\0' && fold(pc) == fold(*s)) {
          ++p;
          ++s;
          continue;
        }
        break;
      }
    } else {
      // The string is used up. Only a run of stars can still match, and a
      // star which has taken everything cannot take more, so there is no
      // point backtracking.
      while (*p == '*')
        ++p;
      return *p == '\0';
    }

    // The characters did not match. Let the last star take one more, unless
    // that would step over a significant slash.
    if (star_p == nullptr)
      return false;
    if (pathname() && *star_s == '/')
      return false;
    p = star_p + 1;
    s = ++star_s;
  }
}

} // namespace fnmatch_internal
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_FNMATCH_FNMATCH_MATCHER_H
