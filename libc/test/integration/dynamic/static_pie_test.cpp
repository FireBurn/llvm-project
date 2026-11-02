//===-- Test of a static PIE started from rcrt1.o ---------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// A static PIE has no interpreter to relocate it, so rcrt1.o does it before
/// anything reads a pointer from the program's data. The program returns
/// nonzero if a pointer it holds in its data was not relocated.
///
//===----------------------------------------------------------------------===//

static int value = 5;
static int other = 6;

// Their contents are relocations, applied when the program starts.
static int *const pointer = &value;
static int *const table[] = {&value, &other};

static int answer() { return 7; }
static int (*const function)() = answer;

extern "C" int main() {
  if (pointer != &value || *pointer != 5)
    return 1;
  if (table[0] != &value || table[1] != &other || *table[1] != 6)
    return 2;
  if (function != answer || function() != 7)
    return 3;
  return 0;
}
