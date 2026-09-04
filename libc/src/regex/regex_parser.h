//===-- Parser for POSIX regular expressions --------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_REGEX_REGEX_PARSER_H
#define LLVM_LIBC_SRC_REGEX_REGEX_PARSER_H

#include "src/__support/ctype_utils.h"
#include "src/regex/regex_engine.h"

namespace LIBC_NAMESPACE_DECL {
namespace regex {

// Recursive descent parser for both dialects.
//
// The grammars differ only in which characters are operators. In an extended
// expression '(', ')', '|', '+', '?' and '{' are operators and a backslash
// makes them literal; in a basic one it is the other way round. Parsing both
// with one set of routines and a flag keeps the two from drifting apart.
class Parser {
public:
  LIBC_INLINE Parser(const char *pattern, int cflags, Node *nodes,
                     uint32_t node_capacity, CharClass *classes,
                     uint32_t class_capacity)
      : p_(pattern), extended_(cflags & REG_EXTENDED),
        icase_(cflags & REG_ICASE), nodes_(nodes),
        node_capacity_(node_capacity), classes_(classes),
        class_capacity_(class_capacity) {}

  // Returns the root node index, or an error code in `error`.
  LIBC_INLINE int32_t parse(int &error) {
    error = 0;
    int32_t root = parse_alternation(error);
    if (error != 0)
      return NONE;
    if (*p_ != '\0') {
      // A ')' with no '(' reaches here.
      error = (*p_ == ')') ? REG_EPAREN : REG_BADPAT;
      return NONE;
    }
    return root;
  }

  LIBC_INLINE uint32_t node_count() const { return node_used_; }
  LIBC_INLINE uint32_t class_count() const { return class_used_; }
  LIBC_INLINE uint32_t group_count() const { return group_used_; }

private:
  LIBC_INLINE int32_t new_node(Op op, int &error) {
    if (node_used_ >= node_capacity_) {
      error = REG_ESPACE;
      return NONE;
    }
    int32_t index = static_cast<int32_t>(node_used_++);
    if (nodes_ != nullptr) {
      nodes_[index] = Node();
      nodes_[index].op = op;
    }
    return index;
  }

  LIBC_INLINE void set(int32_t index, int32_t left, int32_t right) {
    if (nodes_ != nullptr && index >= 0) {
      nodes_[index].left = left;
      nodes_[index].right = right;
    }
  }

  // Is the character at p_ the operator `op` in this dialect? In a basic
  // expression the operator is spelled with a leading backslash.
  LIBC_INLINE bool at_operator(char op) const {
    if (extended_)
      return *p_ == op;
    return p_[0] == '\\' && p_[1] == op;
  }

  LIBC_INLINE void skip_operator() { p_ += extended_ ? 1 : 2; }

  LIBC_INLINE int32_t parse_alternation(int &error) {
    int32_t left = parse_concat(error);
    if (error != 0)
      return NONE;
    while (at_operator('|')) {
      skip_operator();
      int32_t right = parse_concat(error);
      if (error != 0)
        return NONE;
      int32_t node = new_node(Op::Alt, error);
      if (error != 0)
        return NONE;
      set(node, left, right);
      left = node;
    }
    return left;
  }

  LIBC_INLINE bool at_concat_end() const {
    if (*p_ == '\0')
      return true;
    if (at_operator('|') || at_operator(')'))
      return true;
    return false;
  }

  LIBC_INLINE int32_t parse_concat(int &error) {
    if (at_concat_end()) {
      // An empty branch, as in "a|" or "()", matches the empty string.
      return new_node(Op::Empty, error);
    }
    int32_t left = parse_repeat(error);
    if (error != 0)
      return NONE;
    while (!at_concat_end()) {
      int32_t right = parse_repeat(error);
      if (error != 0)
        return NONE;
      int32_t node = new_node(Op::Concat, error);
      if (error != 0)
        return NONE;
      set(node, left, right);
      left = node;
    }
    return left;
  }

  LIBC_INLINE int32_t wrap_repeat(int32_t atom, int32_t min, int32_t max,
                                  int &error) {
    int32_t node = new_node(Op::Repeat, error);
    if (error != 0)
      return NONE;
    set(node, atom, NONE);
    if (nodes_ != nullptr) {
      nodes_[node].min = min;
      nodes_[node].max = max;
    }
    return node;
  }

  // Parses "{n}", "{n,}" or "{n,m}". p_ is on the character after the brace.
  LIBC_INLINE bool parse_bound(int32_t &min, int32_t &max, int &error) {
    if (!internal::isdigit(*p_)) {
      error = REG_BADBR;
      return false;
    }
    min = 0;
    while (internal::isdigit(*p_)) {
      min = min * 10 + (*p_ - '0');
      if (min > RE_DUP_MAX) {
        error = REG_BADBR;
        return false;
      }
      ++p_;
    }
    max = min;
    if (*p_ == ',') {
      ++p_;
      if (internal::isdigit(*p_)) {
        max = 0;
        while (internal::isdigit(*p_)) {
          max = max * 10 + (*p_ - '0');
          if (max > RE_DUP_MAX) {
            error = REG_BADBR;
            return false;
          }
          ++p_;
        }
      } else {
        max = UNBOUNDED;
      }
    }
    if (max != UNBOUNDED && max < min) {
      error = REG_BADBR;
      return false;
    }
    if (extended_) {
      if (*p_ != '}') {
        error = REG_EBRACE;
        return false;
      }
      ++p_;
    } else {
      if (p_[0] != '\\' || p_[1] != '}') {
        error = REG_EBRACE;
        return false;
      }
      p_ += 2;
    }
    return true;
  }

  LIBC_INLINE int32_t parse_repeat(int &error) {
    int32_t atom = parse_atom(error);
    if (error != 0)
      return NONE;
    for (;;) {
      if (*p_ == '*') {
        ++p_;
        atom = wrap_repeat(atom, 0, UNBOUNDED, error);
      } else if (at_operator('+')) {
        skip_operator();
        atom = wrap_repeat(atom, 1, UNBOUNDED, error);
      } else if (at_operator('?')) {
        skip_operator();
        atom = wrap_repeat(atom, 0, 1, error);
      } else if (at_operator('{')) {
        skip_operator();
        int32_t min = 0, max = 0;
        if (!parse_bound(min, max, error))
          return NONE;
        atom = wrap_repeat(atom, min, max, error);
      } else {
        return atom;
      }
      if (error != 0)
        return NONE;
    }
  }

  // Adds one named class, e.g. "[:alpha:]". p_ is on the '['.
  LIBC_INLINE bool parse_named_class(CharClass &cc, int &error) {
    const char *start = p_ + 2;
    const char *end = start;
    while (*end != '\0' && *end != ':')
      ++end;
    if (end[0] != ':' || end[1] != ']') {
      error = REG_ECTYPE;
      return false;
    }
    const size_t length = static_cast<size_t>(end - start);
    auto named = [&](const char *name) {
      size_t i = 0;
      for (; i < length && name[i] != '\0'; ++i)
        if (name[i] != start[i])
          return false;
      return i == length && name[i] == '\0';
    };
    for (int c = 0; c < 256; ++c) {
      unsigned char uc = static_cast<unsigned char>(c);
      bool member = false;
      const char sc = static_cast<char>(uc);
      if (named("alpha"))
        member = internal::isalpha(sc);
      else if (named("digit"))
        member = internal::isdigit(sc);
      else if (named("alnum"))
        member = internal::isalnum(sc);
      else if (named("upper"))
        member = internal::isupper(sc);
      else if (named("lower"))
        member = internal::islower(sc);
      else if (named("space"))
        member = internal::isspace(sc);
      else if (named("blank"))
        member = uc == ' ' || uc == '\t';
      else if (named("punct"))
        member = internal::isgraph(sc) && !internal::isalnum(sc);
      else if (named("print"))
        member = uc >= 0x20 && uc < 0x7f;
      else if (named("graph"))
        member = internal::isgraph(sc);
      else if (named("cntrl"))
        member = uc < 0x20 || uc == 0x7f;
      else if (named("xdigit"))
        member =
            internal::isdigit(sc) || ((uc | 32) >= 'a' && (uc | 32) <= 'f');
      else {
        error = REG_ECTYPE;
        return false;
      }
      if (member)
        cc.add(uc);
    }
    p_ = end + 2;
    return true;
  }

  LIBC_INLINE int32_t parse_bracket(int &error) {
    if (class_used_ >= class_capacity_) {
      error = REG_ESPACE;
      return NONE;
    }
    CharClass local{};
    CharClass &cc = classes_ != nullptr ? classes_[class_used_] : local;
    cc = CharClass{};

    ++p_; // past '['
    bool negated = false;
    if (*p_ == '^') {
      negated = true;
      ++p_;
    }
    // A ']' first is a literal, not the end of the class.
    bool first = true;
    for (;;) {
      if (*p_ == '\0') {
        error = REG_EBRACK;
        return NONE;
      }
      if (*p_ == ']' && !first)
        break;
      first = false;

      if (p_[0] == '[' && p_[1] == ':') {
        if (!parse_named_class(cc, error))
          return NONE;
        continue;
      }

      unsigned char lo = static_cast<unsigned char>(*p_++);
      // A '-' before the closing bracket is a literal.
      if (*p_ == '-' && p_[1] != ']' && p_[1] != '\0') {
        ++p_;
        unsigned char hi = static_cast<unsigned char>(*p_++);
        if (hi < lo) {
          error = REG_ERANGE;
          return NONE;
        }
        for (int c = lo; c <= hi; ++c)
          cc.add(static_cast<unsigned char>(c));
      } else {
        cc.add(lo);
      }
    }
    ++p_; // past ']'

    if (icase_) {
      // Fold before negating, so a negated class excludes both cases.
      for (int c = 'a'; c <= 'z'; ++c) {
        unsigned char lower = static_cast<unsigned char>(c);
        unsigned char upper = static_cast<unsigned char>(c - 32);
        if (cc.has(lower))
          cc.add(upper);
        else if (cc.has(upper))
          cc.add(lower);
      }
    }
    if (negated) {
      cc.negate();
      // A newline is never matched by a negated class under REG_NEWLINE, but
      // that is decided at match time so the class stays reusable.
    }

    int32_t node = new_node(Op::Class, error);
    if (error != 0)
      return NONE;
    if (nodes_ != nullptr)
      nodes_[node].index = class_used_;
    ++class_used_;
    return node;
  }

  LIBC_INLINE int32_t literal(unsigned char c, int &error) {
    int32_t node = new_node(Op::Char, error);
    if (error != 0)
      return NONE;
    if (nodes_ != nullptr)
      nodes_[node].ch = c;
    return node;
  }

  LIBC_INLINE int32_t parse_atom(int &error) {
    if (at_operator('(')) {
      skip_operator();
      uint32_t group = ++group_used_;
      int32_t inner = parse_alternation(error);
      if (error != 0)
        return NONE;
      if (!at_operator(')')) {
        error = REG_EPAREN;
        return NONE;
      }
      skip_operator();
      int32_t node = new_node(Op::Group, error);
      if (error != 0)
        return NONE;
      set(node, inner, NONE);
      if (nodes_ != nullptr)
        nodes_[node].index = group;
      return node;
    }

    if (*p_ == '[')
      return parse_bracket(error);

    if (*p_ == '.') {
      ++p_;
      return new_node(Op::Any, error);
    }

    // '^' anchors only where an expression may begin in a basic expression,
    // but anywhere in an extended one. Treating it as an anchor everywhere is
    // what every practical implementation does.
    if (*p_ == '^') {
      ++p_;
      return new_node(Op::Bol, error);
    }
    if (*p_ == '$') {
      ++p_;
      return new_node(Op::Eol, error);
    }

    if (*p_ == '\\') {
      char next = p_[1];
      if (next == '\0') {
        error = REG_EESCAPE;
        return NONE;
      }
      // In a basic expression these are operators and were handled above, so
      // reaching here with one means it is being used literally.
      if (next >= '1' && next <= '9') {
        p_ += 2;
        int32_t node = new_node(Op::Backref, error);
        if (error != 0)
          return NONE;
        if (nodes_ != nullptr)
          nodes_[node].index = static_cast<uint32_t>(next - '0');
        return node;
      }
      p_ += 2;
      return literal(static_cast<unsigned char>(next), error);
    }

    if (*p_ == '\0') {
      error = REG_BADPAT;
      return NONE;
    }
    // A repetition operator with nothing to repeat.
    if (*p_ == '*') {
      error = REG_BADRPT;
      return NONE;
    }
    unsigned char c = static_cast<unsigned char>(*p_++);
    return literal(c, error);
  }

  const char *p_;
  bool extended_;
  bool icase_;
  Node *nodes_;
  uint32_t node_capacity_;
  uint32_t node_used_ = 0;
  CharClass *classes_;
  uint32_t class_capacity_;
  uint32_t class_used_ = 0;
  uint32_t group_used_ = 0;
};

} // namespace regex
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_REGEX_REGEX_PARSER_H
