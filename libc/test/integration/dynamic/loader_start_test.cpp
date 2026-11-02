//===-- Test of what the loader hands the program ----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Tests that a program started through the loader sees what it would have
/// seen had the kernel started it: its own arguments and environment, an
/// auxiliary vector that names the loader as the interpreter and the program
/// as the entry point. The program returns nonzero if any check fails.
///
//===----------------------------------------------------------------------===//

#include "check.h"

#include <elf.h>
#include <string.h>
#include <sys/auxv.h>

extern "C" char **environ;
extern "C" void _start();

extern "C" int main(int argc, char **argv, char **envp) {
  CHECK(argc >= 1);
  CHECK(argv[0] != nullptr);
  CHECK(argv[argc] == nullptr);
  CHECK(envp == environ);

  // The kernel put the loader's address in AT_BASE for a program with an
  // interpreter, and the program's own entry point in AT_ENTRY.
  CHECK(getauxval(AT_BASE) != 0);
  CHECK(getauxval(AT_ENTRY) != 0);
  CHECK(getauxval(AT_PHDR) != 0);
  CHECK(getauxval(AT_PHNUM) != 0);
  CHECK(getauxval(AT_PAGESZ) >= 4096);

  // Where the program was entered is its own start, not the loader's.
  CHECK(getauxval(AT_ENTRY) == reinterpret_cast<unsigned long>(&_start));
  return failures == 0 ? 0 : 1;
}
