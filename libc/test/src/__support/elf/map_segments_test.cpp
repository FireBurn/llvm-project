//===-- Unittests for PT_LOAD segment mapping -----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/fcntl_macros.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/close.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/open.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/write.h"
#include "src/__support/elf/map_segments.h"
#include "test/UnitTest/Test.h"

using LIBC_NAMESPACE::elf::map_segments;
using LIBC_NAMESPACE::elf::MappedModule;
using LIBC_NAMESPACE::elf::unmap_module;

namespace {

constexpr size_t PAGE = 4096;
constexpr char PATH[] = "testdata/map_segments.bin";

// Two pages of distinguishable bytes, so a wrongly computed file offset shows
// up as the wrong page rather than as a crash.
constexpr unsigned char FIRST_BYTE = 0xaa;
constexpr unsigned char SECOND_BYTE = 0xbb;

int write_backing_file() {
  auto fd = LIBC_NAMESPACE::linux_syscalls::open(
      PATH, O_CREAT | O_TRUNC | O_WRONLY, 0600);
  if (!fd.has_value())
    return -1;
  static unsigned char page[PAGE];
  for (size_t i = 0; i < PAGE; ++i)
    page[i] = FIRST_BYTE;
  LIBC_NAMESPACE::linux_syscalls::write(fd.value(), page, PAGE);
  for (size_t i = 0; i < PAGE; ++i)
    page[i] = SECOND_BYTE;
  LIBC_NAMESPACE::linux_syscalls::write(fd.value(), page, PAGE);
  LIBC_NAMESPACE::linux_syscalls::close(fd.value());

  auto rd = LIBC_NAMESPACE::linux_syscalls::open(PATH, O_RDONLY, 0);
  return rd.has_value() ? rd.value() : -1;
}

ElfW(Phdr) load(ElfW(Addr) vaddr, ElfW(Off) offset, ElfW(Xword) filesz,
                ElfW(Xword) memsz, ElfW(Word) flags) {
  ElfW(Phdr) p{};
  p.p_type = PT_LOAD;
  p.p_vaddr = vaddr;
  p.p_offset = offset;
  p.p_filesz = filesz;
  p.p_memsz = memsz;
  p.p_flags = flags;
  p.p_align = PAGE;
  return p;
}

} // anonymous namespace

TEST(LlvmLibcElfMapSegmentsTest, RejectsModuleWithNoLoadSegments) {
  ElfW(Phdr) phdrs[1]{};
  phdrs[0].p_type = PT_DYNAMIC;
  auto result = map_segments(-1, phdrs, 1, PAGE);
  EXPECT_FALSE(result.has_value());
}

TEST(LlvmLibcElfMapSegmentsTest, MapsSegmentsAndZeroesBss) {
  int fd = write_backing_file();
  ASSERT_GE(fd, 0);

  // Page 0 read only from file offset 0, page 1 read write from offset PAGE
  // with a second page of .bss behind it.
  ElfW(Phdr) phdrs[] = {
      load(0, 0, PAGE, PAGE, PF_R),
      load(PAGE, PAGE, PAGE, 2 * PAGE, PF_R | PF_W),
  };

  auto result = map_segments(fd, phdrs, 2, PAGE);
  LIBC_NAMESPACE::linux_syscalls::close(fd);
  ASSERT_TRUE(result.has_value());

  MappedModule module = result.value();
  ASSERT_TRUE(module.reservation != nullptr);
  // Three pages: one read only, one read write, one .bss.
  EXPECT_EQ(module.reservation_size, size_t(3 * PAGE));

  const unsigned char *base =
      reinterpret_cast<const unsigned char *>(module.load_bias);
  // File backed contents landed on the right pages.
  EXPECT_EQ(base[0], FIRST_BYTE);
  EXPECT_EQ(base[PAGE - 1], FIRST_BYTE);
  EXPECT_EQ(base[PAGE], SECOND_BYTE);
  EXPECT_EQ(base[2 * PAGE - 1], SECOND_BYTE);
  // The .bss page is zero, not a repeat of the file.
  EXPECT_EQ(base[2 * PAGE], static_cast<unsigned char>(0));
  EXPECT_EQ(base[3 * PAGE - 1], static_cast<unsigned char>(0));

  unmap_module(module);
}

TEST(LlvmLibcElfMapSegmentsTest, PartialPageBssIsZeroed) {
  int fd = write_backing_file();
  ASSERT_GE(fd, 0);

  // filesz stops halfway through the page, so the rest of that page still
  // holds file bytes and has to be cleared.
  ElfW(Phdr) phdrs[] = {load(0, 0, PAGE / 2, PAGE, PF_R | PF_W)};

  auto result = map_segments(fd, phdrs, 1, PAGE);
  LIBC_NAMESPACE::linux_syscalls::close(fd);
  ASSERT_TRUE(result.has_value());

  MappedModule module = result.value();
  const unsigned char *base =
      reinterpret_cast<const unsigned char *>(module.load_bias);
  EXPECT_EQ(base[0], FIRST_BYTE);
  EXPECT_EQ(base[PAGE / 2 - 1], FIRST_BYTE);
  EXPECT_EQ(base[PAGE / 2], static_cast<unsigned char>(0));
  EXPECT_EQ(base[PAGE - 1], static_cast<unsigned char>(0));

  unmap_module(module);
}
