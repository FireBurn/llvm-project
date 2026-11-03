//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the evaluator of the Plural-Forms rule of a message catalog.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC___SUPPORT_LIBINTL_PLURAL_H
#define LLVM_LIBC_SRC___SUPPORT_LIBINTL_PLURAL_H

#include "src/__support/CPP/string_view.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"

#include <stddef.h>

namespace LIBC_NAMESPACE_DECL {
namespace libintl {

// The rule a catalog gives for choosing the form of a message from a count, as
// in "nplurals=2; plural=(n != 1);". The expression is the C one over the
// count n: numbers, ?:, ||, &&, == != < > <= >=, + - * / % and parentheses.
class PluralRule {
  cpp::string_view expr;
  unsigned long nplurals = 2;
  bool valid = false;

  // Recursive descent over the text, one function for each level of
  // precedence. An error ends the evaluation and the rule is not used.
  struct Eval {
    cpp::string_view text;
    size_t pos = 0;
    unsigned long n;
    bool ok = true;
    unsigned depth = 0;

    LIBC_INLINE void skip() {
      while (pos < text.size() &&
             (text[pos] == ' ' || text[pos] == '\t' || text[pos] == '\n'))
        ++pos;
    }
    LIBC_INLINE bool eat(const char *op) {
      skip();
      size_t len = 0;
      while (op[len])
        ++len;
      if (text.size() - pos < len)
        return false;
      for (size_t i = 0; i < len; ++i)
        if (text[pos + i] != op[i])
          return false;
      // A single < or > or = or ! or | or & must not be the start of a longer
      // operator.
      if (len == 1 && pos + 1 < text.size()) {
        char next = text[pos + 1];
        char c = op[0];
        if ((c == '<' || c == '>' || c == '!') && next == '=')
          return false;
        if (c == '=' && next == '=')
          return false;
      }
      pos += len;
      return true;
    }
    LIBC_INLINE unsigned long primary() {
      skip();
      if (pos >= text.size() || ++depth > 64) {
        ok = false;
        return 0;
      }
      unsigned long v = 0;
      char c = text[pos];
      if (c == '(') {
        ++pos;
        v = ternary();
        if (!eat(")"))
          ok = false;
      } else if (c == 'n') {
        ++pos;
        v = n;
      } else if (c >= '0' && c <= '9') {
        while (pos < text.size() && text[pos] >= '0' && text[pos] <= '9') {
          v = v * 10 + static_cast<unsigned long>(text[pos] - '0');
          ++pos;
        }
      } else if (c == '!') {
        ++pos;
        v = !primary();
      } else {
        ok = false;
      }
      --depth;
      return v;
    }
    LIBC_INLINE unsigned long mul() {
      unsigned long v = primary();
      for (;;) {
        if (eat("*")) {
          v *= primary();
        } else if (eat("/")) {
          unsigned long d = primary();
          if (d == 0)
            ok = false;
          else
            v /= d;
        } else if (eat("%")) {
          unsigned long d = primary();
          if (d == 0)
            ok = false;
          else
            v %= d;
        } else {
          return v;
        }
      }
    }
    LIBC_INLINE unsigned long add() {
      unsigned long v = mul();
      for (;;) {
        if (eat("+"))
          v += mul();
        else if (eat("-"))
          v -= mul();
        else
          return v;
      }
    }
    LIBC_INLINE unsigned long relational() {
      unsigned long v = add();
      for (;;) {
        if (eat("<="))
          v = v <= add();
        else if (eat(">="))
          v = v >= add();
        else if (eat("<"))
          v = v < add();
        else if (eat(">"))
          v = v > add();
        else
          return v;
      }
    }
    LIBC_INLINE unsigned long equality() {
      unsigned long v = relational();
      for (;;) {
        if (eat("=="))
          v = v == relational();
        else if (eat("!="))
          v = v != relational();
        else
          return v;
      }
    }
    LIBC_INLINE unsigned long logical_and() {
      unsigned long v = equality();
      while (eat("&&")) {
        unsigned long r = equality();
        v = v && r;
      }
      return v;
    }
    LIBC_INLINE unsigned long logical_or() {
      unsigned long v = logical_and();
      while (eat("||")) {
        unsigned long r = logical_and();
        v = v || r;
      }
      return v;
    }
    LIBC_INLINE unsigned long ternary() {
      unsigned long c = logical_or();
      if (!eat("?"))
        return c;
      unsigned long a = ternary();
      if (!eat(":")) {
        ok = false;
        return 0;
      }
      unsigned long b = ternary();
      return c ? a : b;
    }
  };

  // What follows key in the header up to the next semicolon, or nothing where
  // the header does not have the key.
  LIBC_INLINE static cpp::string_view value_after(cpp::string_view header,
                                                  cpp::string_view key) {
    for (size_t at = 0; at + key.size() <= header.size(); ++at) {
      if (!header.substr(at, key.size()).starts_with(key))
        continue;
      cpp::string_view rest = header.substr(at + key.size());
      size_t end = 0;
      while (end < rest.size() && rest[end] != ';')
        ++end;
      return rest.substr(0, end);
    }
    return cpp::string_view();
  }

public:
  LIBC_INLINE PluralRule() = default;

  // Reads the rule out of the header entry of a catalog, which is the
  // translation of the empty message. Without a usable rule the English one
  // applies.
  LIBC_INLINE explicit PluralRule(cpp::string_view header) {
    cpp::string_view count = value_after(header, "nplurals=");
    expr = value_after(header, "plural=");
    if (count.empty() || expr.empty())
      return;
    unsigned long v = 0;
    for (char c : count) {
      if (c == ' ')
        continue;
      if (c < '0' || c > '9')
        return;
      v = v * 10 + static_cast<unsigned long>(c - '0');
    }
    if (v == 0 || v > 16)
      return;
    nplurals = v;
    valid = true;
  }

  // The index of the form to use for the count, which is always below the
  // number of forms the catalog has.
  LIBC_INLINE unsigned long form(unsigned long n) const {
    if (!valid)
      return n != 1;
    Eval e{expr, 0, n};
    unsigned long v = e.ternary();
    e.skip();
    if (!e.ok || e.pos != expr.size() || v >= nplurals)
      return n != 1 && nplurals > 1 ? 1 : 0;
    return v;
  }

  LIBC_INLINE unsigned long forms() const { return nplurals; }
};

} // namespace libintl
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC___SUPPORT_LIBINTL_PLURAL_H
