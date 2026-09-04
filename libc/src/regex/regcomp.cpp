//===-- Implementation of regcomp -----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/regex/regcomp.h"

#include "hdr/func/free.h"
#include "hdr/func/malloc.h"
#include "hdr/regex_macros.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/regex/regex_engine.h"
#include "src/regex/regex_parser.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, regcomp,
                   (regex_t *__restrict preg, const char *__restrict pattern,
                    int cflags)) {
  if (preg == nullptr || pattern == nullptr)
    return REG_BADPAT;
  preg->__internal = nullptr;
  preg->re_nsub = 0;

  // Parse once with no storage to learn how much is needed, then again to
  // build it. Counting first keeps the whole compiled form in one allocation.
  regex::Parser counter(pattern, cflags, nullptr, 0xffffffu, nullptr,
                        0xffffffu);
  int error = 0;
  counter.parse(error);
  if (error != 0)
    return error;

  const uint32_t nodes = counter.node_count();
  const uint32_t classes = counter.class_count();
  const size_t bytes = sizeof(regex::Compiled) + nodes * sizeof(regex::Node) +
                       classes * sizeof(regex::CharClass);
  void *block = ::malloc(bytes);
  if (block == nullptr)
    return REG_ESPACE;

  auto *compiled = static_cast<regex::Compiled *>(block);
  *compiled = regex::Compiled();
  compiled->nodes = reinterpret_cast<regex::Node *>(static_cast<char *>(block) +
                                                    sizeof(regex::Compiled));
  compiled->classes = reinterpret_cast<regex::CharClass *>(
      reinterpret_cast<char *>(compiled->nodes) + nodes * sizeof(regex::Node));
  compiled->cflags = cflags;

  regex::Parser builder(pattern, cflags, compiled->nodes, nodes,
                        compiled->classes, classes);
  compiled->root = builder.parse(error);
  if (error != 0) {
    ::free(block);
    return error;
  }
  compiled->node_count = builder.node_count();
  compiled->class_count = builder.class_count();
  compiled->group_count = builder.group_count();

  preg->re_nsub = compiled->group_count;
  preg->__internal = block;
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL
