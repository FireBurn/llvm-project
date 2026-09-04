//===-- Backtracking matcher for POSIX regular expressions ------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_REGEX_REGEX_MATCHER_H
#define LLVM_LIBC_SRC_REGEX_REGEX_MATCHER_H

#include "hdr/types/size_t.h"
#include "src/__support/ctype_utils.h"
#include "src/regex/regex_engine.h"

namespace LIBC_NAMESPACE_DECL {
namespace regex {

// Highest subexpression number POSIX requires to be reportable.
constexpr uint32_t MAX_GROUPS = 10;

// Matches a parsed pattern against a string by backtracking.
//
// Each step is given the index of the node to match and a continuation, which
// is what makes alternation and repetition composable: a branch succeeds only
// if the rest of the pattern also succeeds after it, so the engine never
// commits to a choice it cannot undo.
class Matcher {
public:
  struct Span {
    int32_t start = -1;
    int32_t end = -1;
  };

  LIBC_INLINE Matcher(const Compiled &compiled, const char *text, size_t length,
                      int eflags)
      : re_(compiled), text_(text), length_(length),
        notbol_(eflags & REG_NOTBOL), noteol_(eflags & REG_NOTEOL),
        icase_(compiled.cflags & REG_ICASE),
        newline_(compiled.cflags & REG_NEWLINE) {}

  // Finds the leftmost match. Returns false if there is none.
  LIBC_INLINE bool search(size_t from, Span *groups) {
    for (size_t start = from; start <= length_; ++start) {
      for (uint32_t i = 0; i < MAX_GROUPS; ++i)
        groups_[i] = Span();
      steps_ = 0;
      end_ = -1;
      if (match(re_.root, static_cast<int32_t>(start), NONE)) {
        groups[0].start = static_cast<int32_t>(start);
        groups[0].end = end_;
        for (uint32_t i = 1; i < MAX_GROUPS; ++i)
          groups[i] = groups_[i];
        return true;
      }
    }
    return false;
  }

private:
  // A pattern like "(a*)*" can otherwise loop forever on an empty match.
  static constexpr uint64_t STEP_LIMIT = 4000000;

  LIBC_INLINE unsigned char fold(unsigned char c) const {
    return icase_ && internal::isupper(static_cast<char>(c))
               ? static_cast<unsigned char>(c + 32)
               : c;
  }

  LIBC_INLINE bool at_line_start(int32_t pos) const {
    if (pos == 0)
      return !notbol_;
    return newline_ && text_[pos - 1] == '\n';
  }

  LIBC_INLINE bool at_line_end(int32_t pos) const {
    if (static_cast<size_t>(pos) == length_)
      return !noteol_;
    return newline_ && text_[pos] == '\n';
  }

  // A '.' does not match a newline when REG_NEWLINE is set, and neither does
  // a negated bracket expression.
  LIBC_INLINE bool any_matches(unsigned char c) const {
    return !(newline_ && c == '\n');
  }

  // `cont` is the node to match after `node`, or NONE for "the whole pattern
  // has matched here".
  LIBC_INLINE bool match(int32_t node, int32_t pos, int32_t cont) {
    if (++steps_ > STEP_LIMIT)
      return false;

    if (node == NONE)
      return match_cont(pos, cont);

    const Node &n = re_.nodes[node];
    switch (n.op) {
    case Op::Empty:
      return match_cont(pos, cont);

    case Op::Char:
      if (static_cast<size_t>(pos) < length_ &&
          fold(static_cast<unsigned char>(text_[pos])) == fold(n.ch))
        return match_cont(pos + 1, cont);
      return false;

    case Op::Any:
      if (static_cast<size_t>(pos) < length_ &&
          any_matches(static_cast<unsigned char>(text_[pos])))
        return match_cont(pos + 1, cont);
      return false;

    case Op::Class: {
      if (static_cast<size_t>(pos) >= length_)
        return false;
      unsigned char c = static_cast<unsigned char>(text_[pos]);
      if (newline_ && c == '\n' && !re_.classes[n.index].has('\n'))
        return false;
      if (!re_.classes[n.index].has(c))
        return false;
      return match_cont(pos + 1, cont);
    }

    case Op::Bol:
      return at_line_start(pos) ? match_cont(pos, cont) : false;

    case Op::Eol:
      return at_line_end(pos) ? match_cont(pos, cont) : false;

    case Op::Concat: {
      // Matching the left side continues into the right, which continues into
      // whatever followed the pair.
      push(n.right, cont);
      bool ok = match(n.left, pos, top_);
      pop();
      return ok;
    }

    case Op::Alt:
      return match(n.left, pos, cont) || match(n.right, pos, cont);

    case Op::Group: {
      const uint32_t g = n.index;
      Span saved = g < MAX_GROUPS ? groups_[g] : Span();
      if (g < MAX_GROUPS)
        groups_[g].start = pos;
      push_group_close(g, cont);
      bool ok = match(n.left, pos, top_);
      pop();
      if (!ok && g < MAX_GROUPS)
        groups_[g] = saved;
      return ok;
    }

    case Op::Repeat:
      return match_repeat(node, pos, cont, 0);

    case Op::Backref: {
      const uint32_t g = n.index;
      if (g >= MAX_GROUPS || groups_[g].start < 0)
        return false;
      const int32_t len = groups_[g].end - groups_[g].start;
      if (len < 0 || static_cast<size_t>(pos + len) > length_)
        return false;
      for (int32_t i = 0; i < len; ++i)
        if (fold(static_cast<unsigned char>(text_[pos + i])) !=
            fold(static_cast<unsigned char>(text_[groups_[g].start + i])))
          return false;
      return match_cont(pos + len, cont);
    }
    }
    return false;
  }

  // Greedy: try one more repetition before trying to finish, so the longest
  // match wins, which is what POSIX asks for.
  LIBC_INLINE bool match_repeat(int32_t node, int32_t pos, int32_t cont,
                                int32_t done) {
    if (++steps_ > STEP_LIMIT)
      return false;
    const Node &n = re_.nodes[node];

    const bool may_repeat = n.max == UNBOUNDED || done < n.max;
    if (may_repeat) {
      push_repeat(node, cont, done + 1, pos);
      bool ok = match(n.left, pos, top_);
      pop();
      if (ok)
        return true;
    }
    if (done >= n.min)
      return match_cont(pos, cont);
    return false;
  }

  // Continuations are held on an explicit stack so a node index alone can
  // name "what to do next".
  struct Frame {
    enum class Kind : uint8_t { Node, GroupClose, Repeat } kind;
    int32_t node;
    int32_t cont;
    uint32_t group;
    int32_t done;
    int32_t entered;
  };

  LIBC_INLINE void push(int32_t node, int32_t cont) {
    if (depth_ >= MAX_DEPTH) {
      overflow_ = true;
      return;
    }
    frames_[depth_] = Frame{Frame::Kind::Node, node, cont, 0, 0, 0};
    top_ = FRAME_BASE + static_cast<int32_t>(depth_);
    ++depth_;
  }
  LIBC_INLINE void push_group_close(uint32_t g, int32_t cont) {
    if (depth_ >= MAX_DEPTH) {
      overflow_ = true;
      return;
    }
    frames_[depth_] = Frame{Frame::Kind::GroupClose, NONE, cont, g, 0, 0};
    top_ = FRAME_BASE + static_cast<int32_t>(depth_);
    ++depth_;
  }
  LIBC_INLINE void push_repeat(int32_t node, int32_t cont, int32_t done,
                               int32_t entered) {
    if (depth_ >= MAX_DEPTH) {
      overflow_ = true;
      return;
    }
    frames_[depth_] = Frame{Frame::Kind::Repeat, node, cont, 0, done, entered};
    top_ = FRAME_BASE + static_cast<int32_t>(depth_);
    ++depth_;
  }
  LIBC_INLINE void pop() {
    if (depth_ > 0)
      --depth_;
    top_ = depth_ > 0 ? FRAME_BASE + static_cast<int32_t>(depth_ - 1) : NONE;
  }

  LIBC_INLINE bool match_cont(int32_t pos, int32_t cont) {
    if (overflow_)
      return false;
    if (cont == NONE) {
      end_ = pos;
      return true;
    }
    const Frame f = frames_[cont - FRAME_BASE];
    switch (f.kind) {
    case Frame::Kind::Node:
      return match(f.node, pos, f.cont);
    case Frame::Kind::GroupClose: {
      Span saved = f.group < MAX_GROUPS ? groups_[f.group] : Span();
      if (f.group < MAX_GROUPS)
        groups_[f.group].end = pos;
      bool ok = match_cont(pos, f.cont);
      if (!ok && f.group < MAX_GROUPS)
        groups_[f.group] = saved;
      return ok;
    }
    case Frame::Kind::Repeat:
      // An iteration that consumed nothing would repeat forever.
      if (pos == f.entered)
        return f.done > re_.nodes[f.node].min ? match_cont(pos, f.cont) : false;
      return match_repeat(f.node, pos, f.cont, f.done);
    }
    return false;
  }

  static constexpr size_t MAX_DEPTH = 512;
  // Frame indices are offset so they cannot be confused with node indices.
  static constexpr int32_t FRAME_BASE = 1 << 20;

  const Compiled &re_;
  const char *text_;
  size_t length_;
  bool notbol_;
  bool noteol_;
  bool icase_;
  bool newline_;
  Span groups_[MAX_GROUPS];
  Frame frames_[MAX_DEPTH];
  size_t depth_ = 0;
  int32_t top_ = NONE;
  int32_t end_ = -1;
  uint64_t steps_ = 0;
  bool overflow_ = false;
};

} // namespace regex
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_REGEX_REGEX_MATCHER_H
