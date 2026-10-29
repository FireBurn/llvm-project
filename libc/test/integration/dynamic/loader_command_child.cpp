//===-- A program the loader is asked to run ------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Run on its own it does nothing. Run by the loader as a command it checks
/// that it was handed its own arguments and not the loader's. See
/// loader_command_test.cpp.
///
//===----------------------------------------------------------------------===//

#include <string.h>

extern "C" int main(int argc, char **argv) {
  if (argc == 1)
    return 0;
  // ld.so program first second: the loader's own name and the program's are
  // not what this sees as its first argument.
  if (argc != 3)
    return 1;
  if (strcmp(argv[1], "first") != 0 || strcmp(argv[2], "second") != 0)
    return 2;
  return 0;
}
