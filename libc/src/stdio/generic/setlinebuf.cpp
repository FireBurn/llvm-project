//===-- Implementation of setlinebuf --------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/stdio/setlinebuf.h"

#include "hdr/stdio_macros.h"
#include "hdr/types/FILE.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/stdio/setvbuf.h"

namespace LIBC_NAMESPACE_DECL {

// The older spelling of asking for line buffering. It cannot report a
// failure, so one is dropped.
LLVM_LIBC_FUNCTION(void, setlinebuf, (::FILE * stream)) {
  LIBC_NAMESPACE::setvbuf(stream, nullptr, _IOLBF, 0);
}

} // namespace LIBC_NAMESPACE_DECL
