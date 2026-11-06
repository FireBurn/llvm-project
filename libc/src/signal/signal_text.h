//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// The text sys_siglist and sys_signame hold for each signal.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_SIGNAL_SIGNAL_TEXT_H
#define LLVM_LIBC_SRC_SIGNAL_SIGNAL_TEXT_H

#include "hdr/signal_macros.h"
#include "src/__support/StringUtil/platform_signals.h"
#include "src/__support/macros/config.h"
#include "src/signal/signal_names.h"

namespace LIBC_NAMESPACE_DECL {
namespace signal_text {

// Signal 0 and the real time signals have no entry, as in bionic.
constexpr const char *description(int signum) {
  for (size_t i = 0; i < PLATFORM_SIGNALS.size(); ++i)
    if (PLATFORM_SIGNALS[i].num == signum && signum != 0 &&
        signum < SIGRTMIN)
      return PLATFORM_SIGNALS[i].msg.data();
  return nullptr;
}

constexpr const char *name(int signum) {
  for (const signal_names::Name &entry : signal_names::NAMES)
    if (entry.number == signum && signum != 0)
      return entry.name;
  return nullptr;
}

} // namespace signal_text
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_SIGNAL_SIGNAL_TEXT_H
