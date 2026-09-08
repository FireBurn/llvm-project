//===-- Where the zone is read from, on Linux -----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/time/tz/zone_source.h"

#include "hdr/fcntl_macros.h"
#include "hdr/sys_mman_macros.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/mmap.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/munmap.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/open.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/read.h"
#include "src/__support/macros/config.h"
#include "src/unistd/close.h"
#include "src/unistd/environ.h"

namespace LIBC_NAMESPACE_DECL {
namespace tz {

namespace {

// Larger than any zone the database holds, by a wide margin.
constexpr size_t MAX_ZONE_SIZE = 1u << 20;

} // anonymous namespace

bool map_zone_file(const char *path, Mapping &out) {
  auto fd = linux_syscalls::open(path, O_RDONLY | O_CLOEXEC, 0);
  if (!fd.has_value())
    return false;

  // The size is not asked for up front; the file is read into a mapping big
  // enough for any of them, which keeps this to the three calls it makes.
  auto mapping =
      linux_syscalls::mmap(nullptr, MAX_ZONE_SIZE, PROT_READ | PROT_WRITE,
                           MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (!mapping.has_value()) {
    LIBC_NAMESPACE::close(fd.value());
    return false;
  }

  unsigned char *into = static_cast<unsigned char *>(mapping.value());
  size_t filled = 0;
  for (;;) {
    auto got =
        linux_syscalls::read(fd.value(), into + filled, MAX_ZONE_SIZE - filled);
    if (!got.has_value() || got.value() < 0) {
      LIBC_NAMESPACE::close(fd.value());
      linux_syscalls::munmap(mapping.value(), MAX_ZONE_SIZE);
      return false;
    }
    if (got.value() == 0)
      break;
    filled += static_cast<size_t>(got.value());
    if (filled == MAX_ZONE_SIZE)
      break;
  }
  LIBC_NAMESPACE::close(fd.value());

  out.data = mapping.value();
  out.size = filled;
  return true;
}

void unmap_zone_file(Mapping &mapping) {
  if (mapping.data == nullptr)
    return;
  linux_syscalls::munmap(mapping.data, MAX_ZONE_SIZE);
  mapping.data = nullptr;
  mapping.size = 0;
}

const char *tz_from_environment() {
  if (environ == nullptr)
    return nullptr;
  for (char **entry = environ; *entry != nullptr; ++entry) {
    const char *at = *entry;
    if (at[0] == 'T' && at[1] == 'Z' && at[2] == '=')
      return at + 3;
  }
  return nullptr;
}

} // namespace tz
} // namespace LIBC_NAMESPACE_DECL
