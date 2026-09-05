//===-- Implementation of open_memstream ----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/stdio/open_memstream.h"

#include "hdr/func/free.h"
#include "hdr/func/malloc.h"
#include "hdr/func/realloc.h"
#include "hdr/stdio_macros.h"
#include "hdr/types/FILE.h"
#include "hdr/types/off_t.h"
#include "hdr/types/size_t.h"
#include "src/__support/CPP/new.h"
#include "src/__support/File/file.h"
#include "src/__support/alloc-checker.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

namespace {

// A stream which writes into a block it grows as it goes, and tells the
// caller where the block is and how much is in it. The two are written
// whenever the stream is flushed or closed, since the block moves when it
// grows and the caller's pointer would otherwise go stale.
class MemStream : public File {
  char **user_buf;
  size_t *user_size;
  char *memory = nullptr;
  size_t capacity = 0;
  size_t position = 0;
  // How much has been written, which is not the position when the caller
  // has seeked back and overwritten part of it.
  size_t length = 0;

  static FileIOResult stream_write(File *f, const void *data, size_t size);
  static FileIOResult stream_read(File *, void *, size_t) {
    // The stream is write only, which is what the mode says too.
    return FileIOResult(0, EBADF);
  }
  static ErrorOr<off_t> stream_seek(File *f, off_t offset, int whence);
  static int stream_close(File *f);

  // Makes room for |needed| bytes counting the terminator, and points the
  // caller's variables at the block.
  bool reserve(size_t needed);
  // |final| is set when the stream is being closed, which is when what was
  // written past the position stops being visible.
  void publish(bool final);

public:
  MemStream(char **bufp, size_t *sizep, uint8_t *buffer, size_t bufsize)
      : File(&stream_write, &stream_read, &MemStream::stream_seek,
             &stream_close, buffer, bufsize, _IONBF,
             true /* File owns the buffer */,
             static_cast<File::ModeFlags>(File::OpenMode::WRITE)),
        user_buf(bufp), user_size(sizep) {}

  bool start();
};

bool MemStream::reserve(size_t needed) {
  if (needed <= capacity)
    return true;
  size_t grown = capacity == 0 ? 64 : capacity;
  while (grown < needed) {
    // Doubling, with a guard against going round on overflow.
    if (grown > (static_cast<size_t>(-1) / 2))
      return false;
    grown *= 2;
  }
  void *bigger = ::realloc(memory, grown);
  if (bigger == nullptr)
    return false;
  memory = reinterpret_cast<char *>(bigger);
  capacity = grown;
  return true;
}

// What the caller is told is where the position is, not how much was ever
// written: a stream seeked back to the middle reports the middle. The bytes
// past it are still in the block, and the terminator goes after them, until
// the stream is closed and they stop being visible.
void MemStream::publish(bool final) {
  if (memory != nullptr)
    memory[final ? position : length] = '\0';
  *user_buf = memory;
  *user_size = position;
}

bool MemStream::start() {
  if (!reserve(1))
    return false;
  publish(false);
  return true;
}

FileIOResult MemStream::stream_write(File *f, const void *data, size_t size) {
  auto *ms = reinterpret_cast<MemStream *>(f);
  if (size == 0)
    return 0;

  // Room for what is written and for the terminator after it.
  if (ms->position > static_cast<size_t>(-1) - size - 1 ||
      !ms->reserve(ms->position + size + 1))
    return FileIOResult(0, ENOMEM);

  const char *from = reinterpret_cast<const char *>(data);
  for (size_t i = 0; i < size; ++i)
    ms->memory[ms->position + i] = from[i];
  ms->position += size;
  if (ms->position > ms->length)
    ms->length = ms->position;

  ms->publish(false);
  return size;
}

ErrorOr<off_t> MemStream::stream_seek(File *f, off_t offset, int whence) {
  auto *ms = reinterpret_cast<MemStream *>(f);
  off_t base;
  switch (whence) {
  case SEEK_SET:
    base = 0;
    break;
  case SEEK_CUR:
    base = static_cast<off_t>(ms->position);
    break;
  case SEEK_END:
    // The end is where the position is, since that is what the size the
    // caller sees means.
    base = static_cast<off_t>(ms->position);
    break;
  default:
    return Error(EINVAL);
  }

  off_t target = base + offset;
  if (target < 0)
    return Error(EINVAL);

  // Seeking past the end is allowed; the gap reads as zero once something
  // is written beyond it.
  size_t want = static_cast<size_t>(target);
  if (want > ms->length) {
    if (!ms->reserve(want + 1))
      return Error(ENOMEM);
    for (size_t i = ms->length; i < want; ++i)
      ms->memory[i] = '\0';
  }
  ms->position = want;
  // The size the caller sees follows the position, so it is told now
  // rather than waiting for the next write.
  ms->publish(false);
  return target;
}

int MemStream::stream_close(File *f) {
  auto *ms = reinterpret_cast<MemStream *>(f);
  ms->publish(true);
  // The block belongs to the caller from here on, so it is not freed.
  return 0;
}

} // anonymous namespace

// Opens a stream which grows a block of memory to hold what is written to
// it. The caller's two variables are set whenever the stream is flushed or
// closed, and the block is theirs to free.
LLVM_LIBC_FUNCTION(::FILE *, open_memstream, (char **bufp, size_t *sizep)) {
  if (bufp == nullptr || sizep == nullptr) {
    libc_errno = EINVAL;
    return nullptr;
  }

  AllocChecker ac;
  uint8_t *buffer = new (ac) uint8_t[File::DEFAULT_BUFFER_SIZE];
  if (!ac) {
    libc_errno = ENOMEM;
    return nullptr;
  }

  auto *stream =
      new (ac) MemStream(bufp, sizep, buffer, File::DEFAULT_BUFFER_SIZE);
  if (!ac) {
    delete[] buffer;
    libc_errno = ENOMEM;
    return nullptr;
  }
  if (!stream->start()) {
    delete stream;
    libc_errno = ENOMEM;
    return nullptr;
  }

  return reinterpret_cast<::FILE *>(stream);
}

} // namespace LIBC_NAMESPACE_DECL
