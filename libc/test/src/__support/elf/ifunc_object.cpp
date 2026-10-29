//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// A shared object with indirect functions, for the bind test to load.
///
//===----------------------------------------------------------------------===//

extern "C" {

static int answer() { return 42; }
static int other() { return 7; }

// The compiler folds an indirect function whose resolver it can evaluate, and
// then no relocation is left to test, so what this picks depends on a value
// it cannot see.
static volatile int selector = 1;
void *choose() {
  return reinterpret_cast<void *>(selector != 0 ? answer : other);
}

// Found by name, so whoever binds against it has to run the resolver.
int exported_ifunc() __attribute__((ifunc("choose")));

// Hidden, so the linker leaves an IRELATIVE entry in this object for the
// call below rather than a symbol anyone can look up.
__attribute__((visibility("hidden"))) int hidden_ifunc()
    __attribute__((ifunc("choose")));

int call_hidden_ifunc() { return hidden_ifunc(); }
}
