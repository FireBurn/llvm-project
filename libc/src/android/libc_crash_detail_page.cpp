//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of __libc_crash_detail_page.
///
//===----------------------------------------------------------------------===//

#include "src/android/libc_crash_detail_page.h"
#include "src/__support/common.h"
#include "src/__support/crash_detail.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

// bionic gives this to debuggerd through its linker as the crash_detail_page
// of debugger_process_info. Nothing public hands it out, so a program that
// installs the handler without bionic's linker asks for it here.
LLVM_LIBC_FUNCTION(void *, __libc_crash_detail_page, ()) {
  return const_cast<crash_detail::Page *>(crash_detail::newest_page());
}

} // namespace LIBC_NAMESPACE_DECL
