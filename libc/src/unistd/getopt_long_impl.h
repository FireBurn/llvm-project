//===-- Long option parsing shared by getopt_long ---------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_UNISTD_GETOPT_LONG_IMPL_H
#define LLVM_LIBC_SRC_UNISTD_GETOPT_LONG_IMPL_H

#include "include/llvm-libc-types/struct_option.h"
#include "src/__support/CPP/string_view.h"
#include "src/__support/macros/config.h"
#include "src/unistd/getopt_common.h"

#include "hdr/getopt_macros.h"

namespace LIBC_NAMESPACE_DECL {
namespace internal {

// Result of matching one "--name" against the long option table.
struct LongMatch {
  const struct option *opt = nullptr;
  int index = -1;
  // Set when "--name=value" was given.
  const char *inline_arg = nullptr;
  bool ambiguous = false;
};

// Splits "name=value" into its two parts. The value is null when there is no
// '=' at all, which is different from an empty value in "name=".
LIBC_INLINE cpp::string_view split_long(cpp::string_view arg,
                                        const char **value) {
  for (size_t i = 0; i < arg.size(); ++i) {
    if (arg[i] == '=') {
      *value = arg.data() + i + 1;
      return arg.substr(0, i);
    }
  }
  *value = nullptr;
  return arg;
}

// Finds `name` in the table, accepting any unambiguous abbreviation, which is
// what makes "--verb" work for "--verbose".
LIBC_INLINE LongMatch match_long(cpp::string_view name,
                                 const struct option *longopts) {
  LongMatch result;
  if (longopts == nullptr)
    return result;
  for (int i = 0; longopts[i].name != nullptr; ++i) {
    cpp::string_view candidate(longopts[i].name);
    if (!candidate.starts_with(name))
      continue;
    if (candidate.size() == name.size()) {
      // An exact match wins outright, even if it is a prefix of another.
      result.opt = &longopts[i];
      result.index = i;
      result.ambiguous = false;
      return result;
    }
    if (result.opt != nullptr) {
      result.ambiguous = true;
      continue;
    }
    result.opt = &longopts[i];
    result.index = i;
  }
  if (result.ambiguous)
    result.opt = nullptr;
  return result;
}

// Parses one long option. `current` is the argument with its leading dashes
// already removed. Returns the value getopt_long should return.
LIBC_INLINE int parse_long(int argc, char *const argv[],
                           cpp::string_view current,
                           const struct option *longopts, int *longindex,
                           bool colon_first, GetoptContext &ctx) {
  const char *inline_arg = nullptr;
  cpp::string_view name = split_long(current, &inline_arg);

  LongMatch match = match_long(name, longopts);
  if (match.opt == nullptr) {
    if (match.ambiguous)
      ctx.report_error("%s: option '--%s' is ambiguous\n", argv[0],
                       name.data());
    else
      ctx.report_error("%s: unrecognized option '--%s'\n", argv[0],
                       name.data());
    ctx.optind.get()++;
    ctx.optpos.get() = 0;
    return '?';
  }

  ctx.optind.get()++;
  ctx.optpos.get() = 0;
  if (longindex != nullptr)
    *longindex = match.index;

  if (match.opt->has_arg == required_argument) {
    if (inline_arg != nullptr) {
      ctx.optarg.get() = const_cast<char *>(inline_arg);
    } else if (ctx.optind < argc && argv[ctx.optind] != nullptr) {
      ctx.optarg.get() = argv[ctx.optind];
      ctx.optind.get()++;
    } else {
      ctx.report_error("%s: option '--%s' requires an argument\n", argv[0],
                       match.opt->name);
      ctx.optopt.get() = match.opt->val;
      return colon_first ? ':' : '?';
    }
  } else if (match.opt->has_arg == optional_argument) {
    // An optional argument is only ever taken from "--name=value", never from
    // the next argv element.
    ctx.optarg.get() = const_cast<char *>(inline_arg);
  } else {
    if (inline_arg != nullptr) {
      ctx.report_error("%s: option '--%s' doesn't allow an argument\n", argv[0],
                       match.opt->name);
      ctx.optopt.get() = match.opt->val;
      return '?';
    }
    ctx.optarg.get() = nullptr;
  }

  if (match.opt->flag != nullptr) {
    *match.opt->flag = match.opt->val;
    return 0;
  }
  return match.opt->val;
}

// Shared body of getopt_long and getopt_long_only. `long_only` also accepts a
// long option written with a single dash.
LIBC_INLINE int getopt_long_r(int argc, char *const argv[],
                              const char *optstring,
                              const struct option *longopts, int *longindex,
                              bool long_only, GetoptContext &ctx) {
  // Zero is a request to scan the arguments again from the start, not an
  // index: see the note in getopt_r, which this shares the state with.
  if (ctx.optind == 0) {
    ctx.optind.get() = 1;
    ctx.optpos.get() = 0;
  }

  if (ctx.optind >= argc || argv[ctx.optind] == nullptr)
    return -1;

  // Mid-cluster, as in the "b" of "-ab": only short parsing applies.
  if (ctx.optpos != 0)
    return getopt_r(argc, argv, optstring, ctx);

  cpp::string_view arg(argv[ctx.optind]);
  if (arg.size() < 2 || arg[0] != '-')
    return -1;
  if (arg == "--") {
    ctx.optind.get()++;
    return -1;
  }

  const bool colon_first = optstring != nullptr && optstring[0] == ':';

  if (arg[1] == '-')
    return parse_long(argc, argv, arg.substr(2), longopts, longindex,
                      colon_first, ctx);

  if (long_only) {
    // A single dash is tried as a long option first; if nothing matches it
    // falls back to short parsing, so "-n" still works when "n" is in
    // optstring but not in the table.
    const char *ignored = nullptr;
    LongMatch match = match_long(split_long(arg.substr(1), &ignored), longopts);
    if (match.opt != nullptr || match.ambiguous)
      return parse_long(argc, argv, arg.substr(1), longopts, longindex,
                        colon_first, ctx);
  }

  return getopt_r(argc, argv, optstring, ctx);
}

} // namespace internal
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_UNISTD_GETOPT_LONG_IMPL_H
