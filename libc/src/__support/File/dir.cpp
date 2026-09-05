//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of the platform independent Dir class.
///
//===----------------------------------------------------------------------===//

#include "src/__support/File/dir.h"

#include "hdr/errno_macros.h"
#include "src/__support/CPP/mutex.h" // lock_guard
#include "src/__support/CPP/new.h"
#include "src/__support/alloc-checker.h"
#include "src/__support/error_or.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

ErrorOr<Dir *> Dir::fdopen(int fd) {
  LIBC_NAMESPACE::AllocChecker ac;
  Dir *dir = new (ac) Dir(fd);
  if (!ac)
    return LIBC_NAMESPACE::Error(ENOMEM);
  return dir;
}

ErrorOr<Dir *> Dir::open(const char *path) {
  auto fd = platform_opendir(path);
  if (!fd)
    return LIBC_NAMESPACE::Error(fd.error());

  return Dir::fdopen(fd.value());
}

ErrorOr<struct dirent *> Dir::read() {
  cpp::lock_guard lock(mutex);
  if (readptr >= fillsize) {
    auto readsize = platform_fetch_dirents(fd, buffer);
    if (!readsize)
      return LIBC_NAMESPACE::Error(readsize.error());
    fillsize = readsize.value();
    readptr = 0;
  }
  if (fillsize == 0)
    return nullptr;

  cpp::span<uint8_t> buf_span(buffer, BUFSIZE);

  // Only the part before the name is a fixed size. An entry's name is as
  // long as it needs to be and no longer, so a record is almost always
  // shorter than the type, and what has to be there to read one is the part
  // up to the name plus at least the byte that ends it.
  constexpr size_t HEADER_SIZE = __builtin_offsetof(struct dirent, d_name);
  if (fillsize - readptr < HEADER_SIZE + 1)
    return Error(EIO);

  struct dirent *d =
      reinterpret_cast<struct dirent *>(buf_span.subspan(readptr).data());

  size_t reclen = platform_dir_reclen(d);

  if (reclen == 0 || readptr + reclen > fillsize)
    return Error(EIO);

  readptr += reclen;
  offset = platform_dir_offset(d);
  return d;
}

off_t Dir::tell() {
  cpp::lock_guard lock(mutex);
  return offset;
}

int Dir::seek(off_t loc) {
  cpp::lock_guard lock(mutex);
  int retval = platform_seekdir(fd, loc);
  if (retval != 0)
    return retval;
  // The buffered entries are from the old position, so drop them.
  readptr = 0;
  fillsize = 0;
  offset = loc;
  return 0;
}

int Dir::close() {
  {
    cpp::lock_guard lock(mutex);
    int retval = platform_closedir(fd);
    if (retval != 0)
      return retval;
  }
  delete this;
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL
