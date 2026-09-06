//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of pututxline
///
//===----------------------------------------------------------------------===//

#include "src/utmpx/pututxline.h"

#include "hdr/stdio_macros.h"
#include "hdr/types/struct_utmpx.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/lseek.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/write.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/utmpx/getutxid.h"
#include "src/utmpx/utmpx_db.h"
namespace LIBC_NAMESPACE_DECL {

namespace {

bool write_at(int fd, long offset, const struct utmpx *rec) {
  if (!linux_syscalls::lseek(fd, offset, SEEK_SET))
    return false;
  const char *p = reinterpret_cast<const char *>(rec);
  size_t done = 0;
  while (done < sizeof(*rec)) {
    ErrorOr<ssize_t> n =
        linux_syscalls::write(fd, p + done, sizeof(*rec) - done);
    if (!n || *n <= 0)
      return false;
    done += static_cast<size_t>(*n);
  }
  return true;
}

} // anonymous namespace

// A record replaces the first one with the same id, if there is one, and is
// appended otherwise. The search starts from where the iteration is, so a
// caller which has just found the record it means to replace does not have
// to go back to the start.
namespace internal {

struct utmpx *pututxline_impl(const struct utmpx *utmpx) {
  if (utmpx == nullptr) {
    libc_errno = EINVAL;
    return nullptr;
  }
  if (!utmpx_db::rewind() && utmpx_db::fd < 0)
    return nullptr;

  // Where the search left off, so that a match can be written back over
  // itself.
  if (!linux_syscalls::lseek(utmpx_db::fd, 0, SEEK_CUR))
    return nullptr;

  long offset = -1;
  if (internal::getutxid_impl(utmpx) != nullptr) {
    ErrorOr<off_t> after = linux_syscalls::lseek(utmpx_db::fd, 0, SEEK_CUR);
    if (after && *after >= static_cast<off_t>(sizeof(*utmpx)))
      offset = *after - static_cast<off_t>(sizeof(*utmpx));
  }

  if (offset < 0) {
    // Nothing matched, so it goes on the end.
    ErrorOr<off_t> end = linux_syscalls::lseek(utmpx_db::fd, 0, SEEK_END);
    if (!end)
      return nullptr;
    offset = *end;
  }

  if (!write_at(utmpx_db::fd, offset, utmpx))
    return nullptr;

  // The record just written is what is handed back, and the iteration is
  // left just past it.
  utmpx_db::entry = *utmpx;
  linux_syscalls::lseek(utmpx_db::fd,
                        offset + static_cast<long>(sizeof(*utmpx)), SEEK_SET);
  return &utmpx_db::entry;
}

} // namespace internal

LLVM_LIBC_FUNCTION(struct utmpx *, pututxline, (const struct utmpx *utmpx)) {
  return internal::pututxline_impl(utmpx);
}

} // namespace LIBC_NAMESPACE_DECL
