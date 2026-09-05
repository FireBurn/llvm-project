//===-- Implementation of syslog ------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/syslog/syslog.h"

#include "src/__support/arg_list.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/syslog/vsyslog.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(void, syslog, (int priority, const char *format, ...)) {
  va_list vlist;
  va_start(vlist, format);
  LIBC_NAMESPACE::vsyslog(priority, format, vlist);
  va_end(vlist);
}

} // namespace LIBC_NAMESPACE_DECL
