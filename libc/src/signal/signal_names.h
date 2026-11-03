//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the names of the signals sig2str and str2sig use.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_SIGNAL_SIGNAL_NAMES_H
#define LLVM_LIBC_SRC_SIGNAL_SIGNAL_NAMES_H

#include "hdr/signal_macros.h"
#include "hdr/types/size_t.h"
#include "src/__support/macros/attributes.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {
namespace signal_names {

struct Name {
  const char *name;
  int number;
};

// The names without the SIG that POSIX gives, and the one it gives for no
// signal. A signal which only some targets have is left to the macro.
inline constexpr Name NAMES[] = {
    {"EXIT", 0},       {"HUP", SIGHUP},       {"INT", SIGINT},
    {"QUIT", SIGQUIT}, {"ILL", SIGILL},       {"TRAP", SIGTRAP},
    {"ABRT", SIGABRT}, {"BUS", SIGBUS},       {"FPE", SIGFPE},
    {"KILL", SIGKILL}, {"USR1", SIGUSR1},     {"SEGV", SIGSEGV},
    {"USR2", SIGUSR2}, {"PIPE", SIGPIPE},     {"ALRM", SIGALRM},
    {"TERM", SIGTERM}, {"STKFLT", SIGSTKFLT}, {"CHLD", SIGCHLD},
    {"CONT", SIGCONT}, {"STOP", SIGSTOP},     {"TSTP", SIGTSTP},
    {"TTIN", SIGTTIN}, {"TTOU", SIGTTOU},     {"URG", SIGURG},
    {"XCPU", SIGXCPU}, {"XFSZ", SIGXFSZ},     {"VTALRM", SIGVTALRM},
    {"PROF", SIGPROF}, {"WINCH", SIGWINCH},   {"IO", SIGIO},
    {"PWR", SIGPWR},   {"SYS", SIGSYS},
};

// Names that also stand for a signal, but are not the one given back.
inline constexpr Name ALIASES[] = {
    {"POLL", SIGPOLL},
    {"CLD", SIGCHLD},
    {"IOT", SIGABRT},
};

} // namespace signal_names
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_SIGNAL_SIGNAL_NAMES_H
