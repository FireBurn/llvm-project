//===-- Handing libc the auxiliary vector ---------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/sys/auxv/linux/set_auxv.h"

#include "src/__support/OSUtil/linux/auxv.h"

extern "C" void __llvm_libc_set_auxv(const void *auxv) {
  LIBC_NAMESPACE::auxv::Vector::initialize_unsafe(
      static_cast<const LIBC_NAMESPACE::auxv::Entry *>(auxv));
}
