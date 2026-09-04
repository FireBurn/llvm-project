//===-- Implementation of fmemopen ----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/stdio/fmemopen.h"

#include "hdr/errno_macros.h"
#include "hdr/func/free.h"
#include "hdr/func/malloc.h"
#include "hdr/stdint_proxy.h"
#include "hdr/stdio_macros.h"
#include "hdr/types/FILE.h"
#include "hdr/types/size_t.h"
#include "src/__support/CPP/new.h"
#include "src/__support/File/file.h"
#include "src/__support/alloc-checker.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

namespace {

// A stream whose bytes are a block of memory rather than a file. Reading
// and writing move a position within that block, and nothing outside it is
// ever touched.
class MemoryFile : public File {
  char *memory;
  size_t capacity;
  // How much of the block a write may use. A stream which can be written
  // keeps a null terminator at the end of what it has written, so one byte
  // is held back for it.
  size_t write_limit;
  size_t position;
  // How much of the block holds anything, which is what the end of the
  // stream means here and where a null terminator is put after a write.
  size_t length;
  bool owns_memory;

  static FileIOResult memory_write(File *f, const void *data, size_t size);
  static FileIOResult memory_read(File *f, void *data, size_t size);
  static ErrorOr<off_t> memory_seek(File *f, off_t offset, int whence);
  static int memory_close(File *f);

public:
  MemoryFile(char *m, size_t cap, size_t limit, size_t len, size_t pos,
             bool owned, uint8_t *buffer, size_t bufsize, File::ModeFlags mode)
      : File(&memory_write, &memory_read, &MemoryFile::memory_seek,
             &memory_close, buffer, bufsize, _IONBF,
             true /* File owns the buffer */, mode),
        memory(m), capacity(cap), write_limit(limit), position(pos),
        length(len), owns_memory(owned) {}
};

FileIOResult MemoryFile::memory_write(File *f, const void *data, size_t size) {
  auto *mf = reinterpret_cast<MemoryFile *>(f);

  // Whatever will not fit is dropped rather than reported: the block is all
  // the room there will ever be, so a short write would be reported on
  // every call from then on. The count says it was all taken.
  size_t room =
      mf->position < mf->write_limit ? mf->write_limit - mf->position : 0;
  size_t to_write = size < room ? size : room;
  const char *from = reinterpret_cast<const char *>(data);
  for (size_t i = 0; i < to_write; ++i)
    mf->memory[mf->position + i] = from[i];
  mf->position += to_write;
  if (mf->position > mf->length)
    mf->length = mf->position;

  // The null terminator goes after what has been written, in the byte held
  // back for it.
  if (mf->length < mf->capacity)
    mf->memory[mf->length] = '\0';
  return size;
}

FileIOResult MemoryFile::memory_read(File *f, void *data, size_t size) {
  auto *mf = reinterpret_cast<MemoryFile *>(f);
  if (mf->position >= mf->length)
    return 0; // The end of what there is to read.
  size_t left = mf->length - mf->position;
  size_t to_read = size < left ? size : left;
  char *to = reinterpret_cast<char *>(data);
  for (size_t i = 0; i < to_read; ++i)
    to[i] = mf->memory[mf->position + i];
  mf->position += to_read;
  return to_read;
}

ErrorOr<off_t> MemoryFile::memory_seek(File *f, off_t offset, int whence) {
  auto *mf = reinterpret_cast<MemoryFile *>(f);
  off_t base;
  switch (whence) {
  case SEEK_SET:
    base = 0;
    break;
  case SEEK_CUR:
    base = static_cast<off_t>(mf->position);
    break;
  case SEEK_END:
    base = static_cast<off_t>(mf->length);
    break;
  default:
    return Error(EINVAL);
  }
  off_t wanted = base + offset;
  // The position may sit at the end of the block but not past it, and never
  // before its start.
  if (wanted < 0 || static_cast<size_t>(wanted) > mf->capacity)
    return Error(EINVAL);
  mf->position = static_cast<size_t>(wanted);
  return wanted;
}

int MemoryFile::memory_close(File *f) {
  auto *mf = reinterpret_cast<MemoryFile *>(f);
  if (mf->owns_memory)
    ::free(mf->memory);
  delete mf;
  return 0;
}

} // anonymous namespace

LLVM_LIBC_FUNCTION(::FILE *, fmemopen,
                   (void *buf, size_t size, const char *mode)) {
  if (mode == nullptr) {
    libc_errno = EINVAL;
    return nullptr;
  }

  File::ModeFlags flags = File::mode_flags(mode);
  if (flags == 0) {
    libc_errno = EINVAL;
    return nullptr;
  }

  // A null buffer means the stream finds its own, which it then frees when
  // it is closed. Such a stream can only be written to and read back.
  bool owned = buf == nullptr;
  char *memory = reinterpret_cast<char *>(buf);
  if (owned) {
    memory = reinterpret_cast<char *>(::malloc(size == 0 ? 1 : size));
    if (memory == nullptr) {
      libc_errno = ENOMEM;
      return nullptr;
    }
    memory[0] = '\0';
  }

  // Where the position starts and how much is already there depend on the
  // mode: appending starts at whatever is in the buffer, writing throws it
  // away, and reading takes all of it.
  const bool append =
      (flags & static_cast<File::ModeFlags>(File::OpenMode::APPEND)) != 0;
  const bool write =
      (flags & static_cast<File::ModeFlags>(File::OpenMode::WRITE)) != 0;

  // A stream which can be written keeps the last byte for a terminator.
  const bool writable =
      write || append ||
      (flags & static_cast<File::ModeFlags>(File::OpenMode::PLUS)) != 0;
  size_t limit = writable && size > 0 ? size - 1 : size;

  size_t length = size;
  size_t position = 0;
  if (append) {
    // The length is up to the first null, or the whole block if there is
    // none, and that is also where writing carries on from.
    length = 0;
    while (length < size && memory[length] != '\0')
      ++length;
    position = length;
  } else if (write) {
    length = 0;
    if (!owned && size > 0)
      memory[0] = '\0';
  }

  uint8_t *buffer;
  {
    AllocChecker ac;
    buffer = new (ac) uint8_t[File::DEFAULT_BUFFER_SIZE];
    if (!ac) {
      if (owned)
        ::free(memory);
      libc_errno = ENOMEM;
      return nullptr;
    }
  }
  AllocChecker ac;
  auto *file = new (ac) MemoryFile(memory, size, limit, length, position, owned,
                                   buffer, File::DEFAULT_BUFFER_SIZE, flags);
  if (!ac) {
    delete[] buffer;
    if (owned)
      ::free(memory);
    libc_errno = ENOMEM;
    return nullptr;
  }
  return reinterpret_cast<::FILE *>(file);
}

} // namespace LIBC_NAMESPACE_DECL
