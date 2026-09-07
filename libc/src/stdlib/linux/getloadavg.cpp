//===-- Linux implementation of getloadavg --------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/stdlib/getloadavg.h"

#include "hdr/types/struct_sysinfo.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/sysinfo.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"

#include <linux/sysinfo.h> // For SI_LOAD_SHIFT.

namespace LIBC_NAMESPACE_DECL {

// How many the kernel keeps: the averages over one, five and fifteen minutes.
constexpr int AVAILABLE = 3;

LLVM_LIBC_FUNCTION(int, getloadavg, (double *loadavg, int nelem)) {
  if (nelem <= 0)
    return 0;
  if (loadavg == nullptr)
    return -1;
  if (nelem > AVAILABLE)
    nelem = AVAILABLE;

  struct sysinfo info;
  if (!linux_syscalls::sysinfo(&info))
    return -1;

  // The kernel keeps these as fixed point numbers, with the point where
  // SI_LOAD_SHIFT says.
  constexpr double SCALE = static_cast<double>(1 << SI_LOAD_SHIFT);
  for (int i = 0; i < nelem; ++i)
    loadavg[i] = static_cast<double>(info.loads[i]) / SCALE;

  return nelem;
}

} // namespace LIBC_NAMESPACE_DECL
