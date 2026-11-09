//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of android_get_LD_LIBRARY_PATH.
///
//===----------------------------------------------------------------------===//

#include "src/android/android_get_LD_LIBRARY_PATH.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/stdlib/environ_internal.h"

namespace LIBC_NAMESPACE_DECL {

// The path the default namespace searches is the environment's, which is
// where android_update_LD_LIBRARY_PATH puts what it is given.
LLVM_LIBC_FUNCTION(void, android_get_LD_LIBRARY_PATH,
                   (char *buffer, size_t buffer_size)) {
  if (buffer == nullptr || buffer_size == 0)
    return;
  const char *path =
      internal::EnvironmentManager::get_instance().get("LD_LIBRARY_PATH");
  size_t i = 0;
  for (; path != nullptr && path[i] != '\0' && i + 1 < buffer_size; ++i)
    buffer[i] = path[i];
  buffer[i] = '\0';
}

} // namespace LIBC_NAMESPACE_DECL
