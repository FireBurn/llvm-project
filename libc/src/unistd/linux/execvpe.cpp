//===-- Linux implementation of execvpe -----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/unistd/execvpe.h"

#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/unistd/linux/exec_path_search.h"

namespace LIBC_NAMESPACE_DECL {

// execvp with the environment given rather than taken from environ.
LLVM_LIBC_FUNCTION(int, execvpe,
                   (const char *file, char *const argv[], char *const envp[])) {
  return exec_search::run(file, argv, envp);
}

} // namespace LIBC_NAMESPACE_DECL
