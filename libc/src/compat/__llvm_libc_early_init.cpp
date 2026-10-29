//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of __llvm_libc_early_init
///
//===----------------------------------------------------------------------===//

#include "src/compat/__llvm_libc_early_init.h"

#include "src/__support/OSUtil/linux/auxv.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/compat/__llvm_libc_init_main_thread.h"
#include "src/compat/program_name.h"
#include "src/unistd/environ.h"

namespace LIBC_NAMESPACE_DECL {

// What a loader tells libc before it runs the initialisers of anything it
// loaded, in one call. An initialiser may read the environment and change
// it, ask for the auxiliary vector, take the program's name, or record which
// thread it is on, as Qt does to tell its threads apart, so all of these have
// to be in place first.
//
// `envp` becomes `environ`, which the startup code then leaves alone, and
// `auxv` is the vector on the stack the program was started with. That is not
// always the one the kernel first passed: something that ran before the
// program may have rewritten it, as Wine's preloader does. Either may be null
// to leave what libc already has.
//
// Calling this more than once is harmless.
LLVM_LIBC_FUNCTION(int, __llvm_libc_early_init,
                   (int argc, char **argv, char **envp, const void *auxv)) {
  if (auxv != nullptr)
    auxv::Vector::initialize_unsafe(static_cast<const auxv::Entry *>(auxv));
  if (envp != nullptr)
    environ = envp;
  if (argc > 0 && argv != nullptr && argv[0] != nullptr)
    set_program_name(argv[0]);
  return __llvm_libc_init_main_thread();
}

} // namespace LIBC_NAMESPACE_DECL
