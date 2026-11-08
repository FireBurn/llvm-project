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
#include "hdr/time_macros.h"
#include "hdr/types/struct_sockaddr_un.h"
#include "hdr/types/struct_stat.h"
#include "hdr/types/struct_timespec.h"
#include "hdr/types/struct_timeval.h"
#include "src/__support/CPP/optional.h"
#include "src/__support/OSUtil/linux/stat/stat_via_statx.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/close.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/connect.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/fsetxattr.h"
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
#include "src/stdlib/getenv.h"
#include "src/sys/system_properties/property_area.h"
#include "src/sys/system_properties/property_info.h"

namespace LIBC_NAMESPACE_DECL {
namespace system_properties {

constexpr size_t NAME_MAX_LEN = 32;
constexpr const char DEFAULT_FILENAME[] = "/dev/__properties__";
constexpr const char SERVICE_SOCKET[] = "/dev/socket/property_service";
constexpr uint32_t MSG_SETPROP2 = 0x00020001;
constexpr int SERVICE_TIMEOUT_MS = 250;

// Where the areas are when there is a directory of them: the file that tells
// which context a property is in, the area that holds the serial everything
// waits on, and then one area for each context, named after it.
constexpr char INFO_FILE[] = "property_info";
constexpr char SERIAL_FILE[] = "properties_serial";
constexpr char SERIAL_CONTEXT[] = "u:object_r:properties_serial:s0";
constexpr char SELINUX_XATTR[] = "security.selinux";
constexpr size_t MAX_CONTEXTS = 512;
constexpr char APPCOMPAT_PREFIX[] = "ro.appcompat_override.";
constexpr char APPCOMPAT_ENV[] = "BIONIC_APPCOMPAT_OVERRIDE";

struct State {
  // The only area, or the one with the serial when there is a directory.
  Area *area = nullptr;
  bool writable = false;
  bool custom_filename = false;
  bool directory = false;
  // Whether a property reads from its ro.appcompat_override. twin first, which
  // the zygote turns on for the apps it starts.
  bool appcompat_override = false;
  bool appcompat_override_checked = false;
  PropertyInfo info;
  Area *contexts[MAX_CONTEXTS] = {};
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

LIBC_INLINE bool join(char *out, const char *directory, const char *leaf) {
  const size_t directory_length = internal::string_length(directory);
  const size_t leaf_length = internal::string_length(leaf);
  if (directory_length + 1 + leaf_length >= PATH_MAX)
    return false;
  inline_memcpy(out, directory, directory_length);
  out[directory_length] = '/';
  inline_memcpy(out + directory_length + 1, leaf, leaf_length + 1);
  return true;
}

LIBC_INLINE bool is_directory(const char *path) {
  struct stat info = {};
  auto status = internal::stat_via_statx(AT_FDCWD, path, 0, &info);
  return status && S_ISDIR(info.st_mode);
}

// Creates a file for an area and maps it to write, which only the process that
// serves the properties does. It is made once: a file that is already there is
// somebody else's. The label is what lets the programs that may read it do so;
// where there is no SELinux to give one, the file is left as it is.
LIBC_INLINE Area *create_file(const char *path, const char *label) {
  auto fd = linux_syscalls::open(
      path, O_RDWR | O_CREAT | O_NOFOLLOW | O_CLOEXEC | O_EXCL, 0444);
  if (!fd) {
    libc_errno = fd.error();
    return nullptr;
  }
  if (label != nullptr)
    (void)linux_syscalls::fsetxattr(fd.value(), SELINUX_XATTR, label,
                                    internal::string_length(label) + 1, 0);
  auto sized = linux_syscalls::ftruncate(fd.value(), AREA_SIZE);
  if (!sized) {
    libc_errno = sized.error();
    (void)linux_syscalls::close(fd.value());
    return nullptr;
  }
  auto mapped = linux_syscalls::mmap(nullptr, AREA_SIZE, PROT_READ | PROT_WRITE,
                                     MAP_SHARED, fd.value(), 0);
  (void)linux_syscalls::close(fd.value());
  if (!mapped) {
    libc_errno = mapped.error();
    return nullptr;
  }
  Area *area = static_cast<Area *>(mapped.value());
  area->init();
  return area;
}

// Maps a file to read. Anything another user could have written to is not
// trusted, as a program decides what to do by what it says. Gives the size of
// what was mapped, which is all of the file unless it is an area.
LIBC_INLINE void *map_trusted(const char *path, size_t minimum, bool whole,
                              size_t *mapped_size) {
  auto fd = linux_syscalls::open(path, O_RDONLY | O_NOFOLLOW | O_CLOEXEC, 0);
  if (!fd) {
    libc_errno = fd.error();
    return nullptr;
  }
  struct stat info = {};
  auto status = internal::stat_via_statx(fd.value(), "", AT_EMPTY_PATH, &info);
  if (!status) {
    libc_errno = status.error();
    (void)linux_syscalls::close(fd.value());
    return nullptr;
  }
  const bool trusted =
      (info.st_uid == 0 || info.st_uid == linux_syscalls::getuid()) &&
      (info.st_mode & (S_IWGRP | S_IWOTH)) == 0 &&
      static_cast<size_t>(info.st_size) >= minimum;
  if (!trusted) {
    libc_errno = EPERM;
    (void)linux_syscalls::close(fd.value());
    return nullptr;
  }
  const size_t length = whole ? static_cast<size_t>(info.st_size) : minimum;
  auto mapped = linux_syscalls::mmap(nullptr, length, PROT_READ, MAP_SHARED,
                                     fd.value(), 0);
  (void)linux_syscalls::close(fd.value());
  if (!mapped) {
    libc_errno = mapped.error();
    return nullptr;
  }
  if (mapped_size != nullptr)
    *mapped_size = length;
  return mapped.value();
}

LIBC_INLINE Area *map_area_file(const char *path) {
  Area *area =
      static_cast<Area *>(map_trusted(path, AREA_SIZE, false, nullptr));
  if (area != nullptr && !area->valid()) {
    libc_errno = EINVAL;
    return nullptr;
  }
  return area;
}

// Reads the file that says where each property goes.
LIBC_INLINE bool map_info(const char *directory) {
  State &s = state();
  char path[PATH_MAX];
  size_t size = 0;
  if (!join(path, directory, INFO_FILE))
    return false;
  void *data = map_trusted(path, sizeof(uint32_t), true, &size);
  if (data == nullptr)
    return false;
  s.info = PropertyInfo(data, size);
  if (!s.info.valid()) {
    libc_errno = EINVAL;
    return false;
  }
  return true;
}

// Creates the areas of a directory, for every context the info file names.
LIBC_INLINE int create_directory() {
  State &s = state();
  char path[PATH_MAX];
  if (!map_info(filename()))
    return -1;
  if (!join(path, filename(), SERIAL_FILE)) {
    libc_errno = ENAMETOOLONG;
    return -1;
  }
  s.area = create_file(path, SERIAL_CONTEXT);
  if (s.area == nullptr)
    return -1;
  const uint32_t count = s.info.num_contexts();
  for (uint32_t i = 0; i < count && i < MAX_CONTEXTS; ++i) {
    const char *context = s.info.context(i);
    if (context == nullptr || !join(path, filename(), context)) {
      libc_errno = EINVAL;
      return -1;
    }
    s.contexts[i] = create_file(path, context);
    if (s.contexts[i] == nullptr)
      return -1;
  }
  s.directory = true;
  s.writable = true;
  return 0;
}

// Creates the area, or the directory of them.
LIBC_INLINE int create_area() {
  State &s = state();
  if (is_directory(filename()))
    return create_directory();
  s.area = create_file(filename(), nullptr);
  if (s.area == nullptr)
    return -1;
  s.writable = true;
  return 0;
}

LIBC_INLINE int map_directory() {
  State &s = state();
  char path[PATH_MAX];
  if (!map_info(filename()))
    return -1;
  if (!join(path, filename(), SERIAL_FILE)) {
    libc_errno = ENAMETOOLONG;
    return -1;
  }
  s.area = map_area_file(path);
  if (s.area == nullptr)
    return -1;
  s.directory = true;
  s.writable = false;
  return 0;
}

// Maps the area, or the directory of them, to read.
LIBC_INLINE int map_area() {
  State &s = state();
  if (is_directory(filename()))
    return map_directory();
  Area *area = map_area_file(filename());
  if (area == nullptr)
    return -1;
  s.area = area;
  s.writable = false;
  return 0;
}

// The area is mapped the first time something needs it. With a directory of
// them this is the one that holds the serial of all of them.
LIBC_INLINE Area *area() {
  State &s = state();
  if (s.area == nullptr && map_area() != 0)
    return nullptr;
  return s.area;
}

// The name with the override prefix in front of it, if that fits.
LIBC_INLINE bool join_name(char *out, size_t size, const char *name) {
  const size_t prefix = sizeof(APPCOMPAT_PREFIX) - 1;
  const size_t length = internal::string_length(name);
  if (prefix + length + 1 > size)
    return false;
  inline_memcpy(out, APPCOMPAT_PREFIX, prefix);
  inline_memcpy(out + prefix, name, length + 1);
  return true;
}

// The area that a property is kept in, with the area of its context mapped the
// first time one is looked for.
LIBC_INLINE Area *area_for(const char *name) {
  State &s = state();
  if (area() == nullptr)
    return nullptr;
  if (!s.directory)
    return s.area;
  const uint32_t index = s.info.context_of(name);
  if (index == NO_INDEX || index >= MAX_CONTEXTS) {
    libc_errno = ENOENT;
    return nullptr;
  }
  if (s.contexts[index] == nullptr) {
    char path[PATH_MAX];
    const char *context = s.info.context(index);
    if (context == nullptr || !join(path, filename(), context)) {
      libc_errno = ENOENT;
      return nullptr;
    }
    s.contexts[index] = map_area_file(path);
  }
  return s.contexts[index];
}

// The property of the name, or the override of it when the overrides are on
// and there is one.
LIBC_INLINE const PropInfo *find_property(const char *name) {
  State &s = state();
  if (!s.appcompat_override_checked) {
    s.appcompat_override_checked = true;
    s.appcompat_override = LIBC_NAMESPACE::getenv(APPCOMPAT_ENV) != nullptr;
  }
  if (s.appcompat_override) {
    char overridden[PATH_MAX];
    if (join_name(overridden, sizeof(overridden), name)) {
      Area *a = area_for(overridden);
      const PropInfo *found = a == nullptr ? nullptr : a->find(overridden);
      if (found != nullptr)
        return found;
    }
  }
  Area *a = area_for(name);
  return a == nullptr ? nullptr : a->find(name);
}

// Calls the function with every area there is. An area that cannot be mapped,
// as the programs that read this may not be allowed to, is left out.
template <typename Function> LIBC_INLINE bool for_each_area(Function function) {
  State &s = state();
  if (area() == nullptr)
    return false;
  if (!s.directory) {
    function(s.area);
    return true;
  }
  const uint32_t count = s.info.num_contexts();
  for (uint32_t i = 0; i < count && i < MAX_CONTEXTS; ++i) {
    if (s.contexts[i] == nullptr) {
      char path[PATH_MAX];
      const char *context = s.info.context(i);
      if (context == nullptr || !join(path, filename(), context))
        continue;
      // Leaving one out is not a failure of the walk.
      const int saved_errno = libc_errno;
      s.contexts[i] = map_area_file(path);
      libc_errno = saved_errno;
    }
    if (s.contexts[i] != nullptr)
      function(s.contexts[i]);
  }
  return true;
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
    const char *source = serial_dirty(serial)
                             ? area_for(info->name())->dirty_backup_area()
                             : info->value;
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
  Area *target = area_for(name);
  if (target == nullptr)
    return -1;
  if (target->find(name) != nullptr) {
    libc_errno = EEXIST;
    return -1;
  }
  if (!target->add(name, name_length, value, value_length)) {
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
  Area *holder = area_for(info->name());
  if (holder == nullptr)
    return -1;
  inline_memcpy(holder->dirty_backup_area(), info->value,
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
