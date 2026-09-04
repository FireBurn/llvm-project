//===-- Implementation of execl -------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/unistd/execl.h"

#include "hdr/types/size_t.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/unistd/execv.h"

#include <stdarg.h>

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, execl, (const char *path, const char *arg0, ...)) {
  va_list varargs, varargs_copy;
  va_start(varargs, arg0);
  va_copy(varargs_copy, varargs);

  size_t argc = 1;
  while (va_arg(varargs, const char *) != nullptr)
    ++argc;
  va_end(varargs);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wvla"
#if __has_warning("-Wvla-cxx-extension")
#pragma GCC diagnostic ignored "-Wvla-cxx-extension"
#endif
  char *argv[argc + 1];
#pragma GCC diagnostic pop
  argv[0] = const_cast<char *>(arg0);

  for (size_t i = 1; i <= argc; ++i)
    argv[i] = va_arg(varargs_copy, char *);
  va_end(varargs_copy);

  return LIBC_NAMESPACE::execv(path, argv);
}

} // namespace LIBC_NAMESPACE_DECL
