//===-- Implementation of pututxline --------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/utmpx/pututxline.h"

#include "hdr/stdio_macros.h"
#include "hdr/types/struct_utmpx.h"
#include "src/__support/OSUtil/syscall.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/utmpx/getutxid.h"
#include "src/utmpx/utmpx_db.h"
#include <sys/syscall.h>

namespace LIBC_NAMESPACE_DECL {

namespace {

bool write_at(int fd, long offset, const struct utmpx *rec) {
  if (LIBC_NAMESPACE::syscall_impl<long>(SYS_lseek, fd, offset, SEEK_SET) < 0)
    return false;
  const char *p = reinterpret_cast<const char *>(rec);
  size_t done = 0;
  while (done < sizeof(*rec)) {
    long n = LIBC_NAMESPACE::syscall_impl<long>(SYS_write, fd, p + done,
                                                sizeof(*rec) - done);
    if (n <= 0)
      return false;
    done += static_cast<size_t>(n);
  }
  return true;
}

} // anonymous namespace

// A record replaces the first one with the same id, if there is one, and is
// appended otherwise. The search starts from where the iteration is, so a
// caller which has just found the record it means to replace does not have
// to go back to the start.
LLVM_LIBC_FUNCTION(struct utmpx *, pututxline, (const struct utmpx *utmpx)) {
  if (utmpx == nullptr) {
    libc_errno = EINVAL;
    return nullptr;
  }
  if (!utmpx_db::rewind() && utmpx_db::fd < 0)
    return nullptr;

  // Where the search left off, so that a match can be written back over
  // itself.
  long here =
      LIBC_NAMESPACE::syscall_impl<long>(SYS_lseek, utmpx_db::fd, 0, SEEK_CUR);
  if (here < 0)
    return nullptr;

  long offset = -1;
  if (LIBC_NAMESPACE::getutxid(utmpx) != nullptr) {
    long after = LIBC_NAMESPACE::syscall_impl<long>(SYS_lseek, utmpx_db::fd, 0,
                                                    SEEK_CUR);
    if (after >= static_cast<long>(sizeof(*utmpx)))
      offset = after - static_cast<long>(sizeof(*utmpx));
  }

  if (offset < 0) {
    // Nothing matched, so it goes on the end.
    offset = LIBC_NAMESPACE::syscall_impl<long>(SYS_lseek, utmpx_db::fd, 0,
                                                SEEK_END);
    if (offset < 0)
      return nullptr;
  }

  if (!write_at(utmpx_db::fd, offset, utmpx))
    return nullptr;

  // The record just written is what is handed back, and the iteration is
  // left just past it.
  utmpx_db::entry = *utmpx;
  LIBC_NAMESPACE::syscall_impl<long>(SYS_lseek, utmpx_db::fd,
                                     offset + static_cast<long>(sizeof(*utmpx)),
                                     SEEK_SET);
  return &utmpx_db::entry;
}

} // namespace LIBC_NAMESPACE_DECL
