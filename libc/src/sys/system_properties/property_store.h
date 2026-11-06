//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// The process's view of the system properties.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_SYS_SYSTEM_PROPERTIES_PROPERTY_STORE_H
#define LLVM_LIBC_SRC_SYS_SYSTEM_PROPERTIES_PROPERTY_STORE_H

#include "hdr/errno_macros.h"
#include "hdr/fcntl_macros.h"
#include "hdr/limits_macros.h"
#include "hdr/sys_mman_macros.h"
#include "hdr/sys_socket_macros.h"
#include "hdr/sys_stat_macros.h"
#include "hdr/types/struct_sockaddr_un.h"
#include "hdr/types/struct_stat.h"
#include "hdr/time_macros.h"
#include "hdr/types/struct_timespec.h"
#include "hdr/types/struct_timeval.h"
#include "src/__support/CPP/optional.h"
#include "src/__support/OSUtil/linux/stat/stat_via_statx.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/close.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/connect.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/ftruncate.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/getuid.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/mmap.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/open.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/read.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/setsockopt.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/socket.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/write.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/__support/threads/linux/futex_utils.h"
#include "src/__support/time/clock_gettime.h"
#include "src/sys/system_properties/property_area.h"

namespace LIBC_NAMESPACE_DECL {
namespace system_properties {

constexpr size_t NAME_MAX_LEN = 32;
constexpr const char DEFAULT_FILENAME[] = "/dev/__properties__";
constexpr const char SERVICE_SOCKET[] = "/dev/socket/property_service";
constexpr uint32_t MSG_SETPROP2 = 0x00020001;
constexpr int SERVICE_TIMEOUT_MS = 250;

struct State {
  Area *area = nullptr;
  bool writable = false;
  bool custom_filename = false;
  char filename[PATH_MAX] = {};
};

LIBC_INLINE State &state() {
  static State instance;
  return instance;
}

LIBC_INLINE const char *filename() {
  State &s = state();
  return s.custom_filename ? s.filename : DEFAULT_FILENAME;
}

LIBC_INLINE bool set_filename(const char *path) {
  State &s = state();
  const size_t length = internal::string_length(path);
  if (length >= sizeof(s.filename))
    return false;
  inline_memcpy(s.filename, path, length + 1);
  s.custom_filename = true;
  return true;
}

LIBC_INLINE bool is_read_only(const char *name) {
  return name[0] == 'r' && name[1] == 'o' && name[2] == '.';
}

// Creates the area, which only the process that serves the properties does.
// It is made once: a file that is already there is somebody else's.
LIBC_INLINE int create_area() {
  State &s = state();
  auto fd = linux_syscalls::open(
      filename(), O_RDWR | O_CREAT | O_NOFOLLOW | O_CLOEXEC | O_EXCL, 0444);
  if (!fd) {
    libc_errno = fd.error();
    return -1;
  }
  auto sized = linux_syscalls::ftruncate(fd.value(), AREA_SIZE);
  if (!sized) {
    libc_errno = sized.error();
    (void)linux_syscalls::close(fd.value());
    return -1;
  }
  auto mapped = linux_syscalls::mmap(nullptr, AREA_SIZE, PROT_READ | PROT_WRITE,
                                     MAP_SHARED, fd.value(), 0);
  (void)linux_syscalls::close(fd.value());
  if (!mapped) {
    libc_errno = mapped.error();
    return -1;
  }
  s.area = static_cast<Area *>(mapped.value());
  s.area->init();
  s.writable = true;
  return 0;
}

// Maps the area to read. Anything another user could have written to is not
// trusted, as a program decides what to do by what it says.
LIBC_INLINE int map_area() {
  State &s = state();
  auto fd = linux_syscalls::open(filename(), O_RDONLY | O_NOFOLLOW | O_CLOEXEC,
                                 0);
  if (!fd) {
    libc_errno = fd.error();
    return -1;
  }
  struct stat info = {};
  auto status = internal::stat_via_statx(fd.value(), "", AT_EMPTY_PATH, &info);
  if (!status) {
    libc_errno = status.error();
    (void)linux_syscalls::close(fd.value());
    return -1;
  }
  const bool trusted =
      (info.st_uid == 0 || info.st_uid == linux_syscalls::getuid()) &&
      (info.st_mode & (S_IWGRP | S_IWOTH)) == 0 &&
      static_cast<size_t>(info.st_size) >= AREA_SIZE;
  if (!trusted) {
    libc_errno = EPERM;
    (void)linux_syscalls::close(fd.value());
    return -1;
  }
  auto mapped = linux_syscalls::mmap(nullptr, AREA_SIZE, PROT_READ, MAP_SHARED,
                                     fd.value(), 0);
  (void)linux_syscalls::close(fd.value());
  if (!mapped) {
    libc_errno = mapped.error();
    return -1;
  }
  Area *area = static_cast<Area *>(mapped.value());
  if (!area->valid()) {
    libc_errno = EINVAL;
    return -1;
  }
  s.area = area;
  s.writable = false;
  return 0;
}

// The area is mapped the first time something needs it.
LIBC_INLINE Area *area() {
  State &s = state();
  if (s.area == nullptr && map_area() != 0)
    return nullptr;
  return s.area;
}

LIBC_INLINE void wake(Word &word) {
  (void)reinterpret_cast<Futex *>(&word)->notify_all(/*is_shared=*/true);
}

// Copies the value out, from the backup when it is being changed as it is read.
// The length is checked against the serial before and after, as that is how a
// change in between shows.
LIBC_INLINE uint32_t read_value(const PropInfo *info, char *value) {
  Word &word = const_cast<Word &>(info->serial);
  uint32_t serial = word.load(cpp::MemoryOrder::ACQUIRE);
  for (;;) {
    const uint32_t length = serial_value_length(serial);
    const char *source =
        serial_dirty(serial) ? area()->dirty_backup_area() : info->value;
    inline_memcpy(value, source, length + 1);
    cpp::atomic_thread_fence(cpp::MemoryOrder::ACQUIRE);
    const uint32_t again = word.load(cpp::MemoryOrder::RELAXED);
    if (again == serial)
      return length;
    serial = again;
  }
}

LIBC_INLINE int add(const char *name, uint32_t name_length, const char *value,
                    uint32_t value_length) {
  if (name_length < 1) {
    libc_errno = EINVAL;
    return -1;
  }
  if (value_length >= VALUE_MAX && !is_read_only(name)) {
    libc_errno = EINVAL;
    return -1;
  }
  State &s = state();
  if (s.area == nullptr || !s.writable) {
    libc_errno = EPERM;
    return -1;
  }
  if (s.area->find(name) != nullptr) {
    libc_errno = EEXIST;
    return -1;
  }
  if (!s.area->add(name, name_length, value, value_length)) {
    libc_errno = ENOSPC;
    return -1;
  }
  s.area->serial().store(
      s.area->serial().load(cpp::MemoryOrder::RELAXED) + 1,
      cpp::MemoryOrder::RELEASE);
  wake(s.area->serial());
  return 0;
}

LIBC_INLINE int update(PropInfo *info, const char *value, uint32_t length) {
  State &s = state();
  if (s.area == nullptr || !s.writable) {
    libc_errno = EPERM;
    return -1;
  }
  if (length >= VALUE_MAX || info->is_long()) {
    libc_errno = EINVAL;
    return -1;
  }
  uint32_t serial = info->serial.load(cpp::MemoryOrder::RELAXED);
  // Readers that arrive while the value is being written take the old one
  // from here.
  inline_memcpy(s.area->dirty_backup_area(), info->value,
                serial_value_length(serial) + 1);
  cpp::atomic_thread_fence(cpp::MemoryOrder::RELEASE);
  serial |= 1;
  info->serial.store(serial, cpp::MemoryOrder::RELAXED);
  inline_memcpy(info->value, value, length);
  info->value[length] = '\0';
  info->serial.store((length << 24) | ((serial + 1) & 0xffffff),
                     cpp::MemoryOrder::RELEASE);
  wake(info->serial);
  s.area->serial().store(
      s.area->serial().load(cpp::MemoryOrder::RELAXED) + 1,
      cpp::MemoryOrder::RELEASE);
  wake(s.area->serial());
  return 0;
}

// Waits for the serial of a property, or of the whole area when there is no
// property, to move on from the one given.
LIBC_INLINE bool wait(const PropInfo *info, uint32_t old_serial,
                      uint32_t *new_serial, const timespec *relative) {
  Area *a = area();
  if (a == nullptr)
    return false;
  Word &word = info != nullptr ? const_cast<Word &>(info->serial) : a->serial();
  cpp::optional<Futex::Timeout> timeout = cpp::nullopt;
  if (relative != nullptr) {
    timespec now = {};
    if (!internal::clock_gettime(CLOCK_MONOTONIC, &now))
      return false;
    timespec deadline = {now.tv_sec + relative->tv_sec,
                         now.tv_nsec + relative->tv_nsec};
    if (deadline.tv_nsec >= 1000000000) {
      deadline.tv_nsec -= 1000000000;
      ++deadline.tv_sec;
    }
    auto converted = Futex::Timeout::from_timespec(deadline, false);
    if (!converted)
      return false;
    timeout = cpp::optional<Futex::Timeout>(converted.value());
  }
  Futex *futex = reinterpret_cast<Futex *>(&word);
  for (;;) {
    const uint32_t current = word.load(cpp::MemoryOrder::ACQUIRE);
    if (current != old_serial && !serial_dirty(current)) {
      if (new_serial != nullptr)
        *new_serial = current;
      return true;
    }
    auto result = futex->wait(current, timeout, /*is_shared=*/true);
    if (!result && result.error() == ETIMEDOUT)
      return false;
  }
}

LIBC_INLINE bool write_all(int fd, const void *data, size_t size) {
  const char *bytes = static_cast<const char *>(data);
  while (size > 0) {
    auto written = linux_syscalls::write(fd, bytes, size);
    if (!written || written.value() <= 0)
      return false;
    bytes += written.value();
    size -= static_cast<size_t>(written.value());
  }
  return true;
}

// Asks the process that serves the properties to set one, as only it may.
LIBC_INLINE int set(const char *name, const char *value) {
  const size_t name_length = internal::string_length(name);
  const size_t value_length = internal::string_length(value);
  if (name_length < 1 || (name_length >= NAME_MAX_LEN && !is_read_only(name)) ||
      (value_length >= VALUE_MAX && !is_read_only(name))) {
    libc_errno = EINVAL;
    return -1;
  }
  auto fd = linux_syscalls::socket(AF_UNIX, SOCK_STREAM | SOCK_CLOEXEC, 0);
  if (!fd) {
    libc_errno = fd.error();
    return -1;
  }
  struct sockaddr_un address = {};
  address.sun_family = AF_UNIX;
  inline_memcpy(address.sun_path, SERVICE_SOCKET, sizeof(SERVICE_SOCKET));
  auto connected = linux_syscalls::connect(
      fd.value(), reinterpret_cast<struct sockaddr *>(&address),
      sizeof(address));
  if (!connected) {
    libc_errno = connected.error();
    (void)linux_syscalls::close(fd.value());
    return -1;
  }
  const timeval limit = {0, SERVICE_TIMEOUT_MS * 1000};
  (void)linux_syscalls::setsockopt(fd.value(), SOL_SOCKET, SO_RCVTIMEO, &limit,
                                   sizeof(limit));

  const uint32_t command = MSG_SETPROP2;
  const uint32_t name_size = static_cast<uint32_t>(name_length);
  const uint32_t value_size = static_cast<uint32_t>(value_length);
  int32_t reply = -1;
  const bool sent = write_all(fd.value(), &command, sizeof(command)) &&
                    write_all(fd.value(), &name_size, sizeof(name_size)) &&
                    write_all(fd.value(), name, name_length) &&
                    write_all(fd.value(), &value_size, sizeof(value_size)) &&
                    write_all(fd.value(), value, value_length);
  bool answered = false;
  if (sent) {
    auto received = linux_syscalls::read(fd.value(), &reply, sizeof(reply));
    answered = received && received.value() == sizeof(reply);
  }
  (void)linux_syscalls::close(fd.value());
  if (!answered) {
    libc_errno = EIO;
    return -1;
  }
  return reply == 0 ? 0 : -1;
}

} // namespace system_properties
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_SYS_SYSTEM_PROPERTIES_PROPERTY_STORE_H
