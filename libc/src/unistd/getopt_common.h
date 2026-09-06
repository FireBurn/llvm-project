//===-- Shared internals of the getopt family -------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_UNISTD_GETOPT_COMMON_H
#define LLVM_LIBC_SRC_UNISTD_GETOPT_COMMON_H

#include "hdr/types/FILE.h"
#include "src/__support/CPP/optional.h"
#include "src/__support/CPP/string_view.h"
#include "src/__support/macros/attributes.h"
#include "src/__support/macros/config.h"
#include "src/stdio/fprintf.h"
#include "src/stdio/stderr.h"

namespace LIBC_NAMESPACE_DECL {

template <typename T> struct RefWrapper {
  RefWrapper() = delete;
  constexpr RefWrapper(T *p) : ptr{p} {}
  constexpr RefWrapper(const RefWrapper &) = default;
  RefWrapper &operator=(const RefWrapper &) = default;
  operator T &() { return *ptr; }
  T &get() { return *ptr; }
  T *ptr;
};

struct GetoptContext {
  RefWrapper<char *> optarg;
  RefWrapper<int> optind;
  RefWrapper<int> optopt;
  RefWrapper<unsigned> optpos;

  RefWrapper<int> opterr;

  FILE *errstream;

  GetoptContext &operator=(const GetoptContext &) = default;

  template <typename... Ts> void report_error(const char *fmt, Ts... ts) {
    if (opterr)
      LIBC_NAMESPACE::fprintf(
          errstream ? errstream
                    : reinterpret_cast<FILE *>(LIBC_NAMESPACE::stderr),
          fmt, ts...);
  }
};

struct OptstringParser {
  using value_type = struct {
    char c;
    bool arg;
  };

  cpp::string_view optstring;

  struct iterator {
    cpp::string_view curr;

    iterator operator++() {
      curr = curr.substr(1);
      return *this;
    }

    bool operator!=(iterator other) { return curr.data() != other.curr.data(); }

    value_type operator*() {
      value_type r{curr.front(), false};
      if (!curr.substr(1).empty() && curr.substr(1).front() == ':') {
        this->operator++();
        r.arg = true;
      }
      return r;
    }
  };

  iterator begin() {
    bool skip = optstring.front() == '-' || optstring.front() == '+' ||
                optstring.front() == ':';
    return {optstring.substr(!!skip)};
  }

  iterator end() { return {optstring.substr(optstring.size())}; }
};

LIBC_INLINE int getopt_r(int argc, char *const argv[], const char *optstring,
                         GetoptContext &ctx) {
  auto failure = [&ctx](int ret = -1) {
    ctx.optpos.get() = 0;
    return ret;
  };

  // A caller which means to scan the arguments again says so by putting
  // optind back to zero, which is not an index into anything: the first
  // argument is the program's own name and was never an option. The scan
  // starts again at the one after it, and whatever was half read is
  // forgotten. Programs which parse their arguments twice rely on this.
  if (ctx.optind == 0) {
    ctx.optind.get() = 1;
    ctx.optpos.get() = 0;
  }

  if (ctx.optind >= argc || !argv[ctx.optind])
    return failure();

  cpp::string_view current =
      cpp::string_view{argv[ctx.optind]}.substr(ctx.optpos);

  auto move_forward = [&current, &ctx] {
    current = current.substr(1);
    ctx.optpos.get()++;
  };

  // If optpos is nonzero, then we are already parsing a valid flag and these
  // need not be checked.
  if (ctx.optpos == 0) {
    if (current[0] != '-')
      return failure();

    if (current == "--") {
      ctx.optind.get()++;
      return failure();
    }

    // Eat the '-' char.
    move_forward();
    if (current.empty())
      return failure();
  }

  auto find_match =
      [current, optstring]() -> cpp::optional<OptstringParser::value_type> {
    for (auto i : OptstringParser{optstring})
      if (i.c == current[0])
        return i;
    return {};
  };

  auto match = find_match();
  if (!match) {
    ctx.report_error("%s: illegal option -- %c\n", argv[0], current[0]);
    ctx.optopt.get() = current[0];
    return failure('?');
  }

  // We've matched so eat that character.
  move_forward();
  if (match->arg) {
    // If we found an option that takes an argument and our current is not over,
    // the rest of current is that argument. Ie, "-cabc" with opstring "c:",
    // then optarg should point to "abc". Otherwise the argument to c will be in
    // the next arg like "-c abc".
    if (!current.empty()) {
      // This const cast is fine because current was already holding a mutable
      // string, it just doesn't have the semantics to note that, we could use
      // span but it doesn't have string_view string niceties.
      ctx.optarg.get() = const_cast<char *>(current.data());
    } else {
      // One char lookahead to see if we ran out of arguments. If so, return ':'
      // if the first character of optstring is ':'. optind must stay at the
      // current value so only increase it after we known there is another arg.
      if (ctx.optind + 1 >= argc || !argv[ctx.optind + 1]) {
        ctx.report_error("%s: option requires an argument -- %c\n", argv[0],
                         match->c);
        return failure(optstring[0] == ':' ? ':' : '?');
      }
      ctx.optarg.get() = argv[++ctx.optind];
    }
    ctx.optind++;
    ctx.optpos.get() = 0;
  } else if (current.empty()) {
    // If this argument is now empty we are safe to move onto the next one.
    ctx.optind++;
    ctx.optpos.get() = 0;
  }

  return match->c;
}

namespace impl {

// The state the public entry points share. getopt and getopt_long parse the
// same argv and must agree on how far through it they are.
extern GetoptContext ctx;

} // namespace impl

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_UNISTD_GETOPT_COMMON_H
