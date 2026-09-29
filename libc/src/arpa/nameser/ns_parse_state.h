//===-- Where ns_parserr has got to in a message ----------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_ARPA_NAMESER_NS_PARSE_STATE_H
#define LLVM_LIBC_SRC_ARPA_NAMESER_NS_PARSE_STATE_H

#include "hdr/types/ns_msg.h"
#include "src/__support/macros/attributes.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {
namespace nameser {

// ns_s_max: the question, answer, authority and additional sections.
constexpr int SECTIONS = 4;

// Moves to the first record of `section`, or past the end of the message
// where that is SECTIONS.
LIBC_INLINE void set_section(ns_msg &handle, int section) {
  handle._sect = section;
  if (section == SECTIONS) {
    handle._rrnum = -1;
    handle._msg_ptr = nullptr;
  } else {
    handle._rrnum = 0;
    handle._msg_ptr = handle._sections[section];
  }
}

} // namespace nameser
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_ARPA_NAMESER_NS_PARSE_STATE_H
