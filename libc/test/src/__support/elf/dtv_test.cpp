//===-- Unittests for a thread's dynamic thread vector --------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/__support/elf/dtv.h"
#include "src/__support/elf/load_module.h"
#include "src/__support/elf/passive_abi.h"
#include "test/UnitTest/Test.h"

using LIBC_NAMESPACE::elf::DynamicThreadVector;
using LIBC_NAMESPACE::elf::load_module;
using LIBC_NAMESPACE::elf::LoadedModule;
using LIBC_NAMESPACE::elf::MappedModule;
using LIBC_NAMESPACE::elf::Module;
using LIBC_NAMESPACE::elf::ModuleSet;
using LIBC_NAMESPACE::elf::unmap_module;

namespace {

constexpr size_t PAGE = 4096;
constexpr const char *LIBC_PATH = LIBC_TEST_SHARED_OBJECT;

// The record is handed in rather than read from the process, which a test
// binary has no loader to fill in. This stands a module in one past those
// loaded at startup, the way an object opened later sits.
class Record {
public:
  Record() {
    set_.modules = modules_;
    set_.mappings = mappings_;
    set_.tls_offsets = offsets_;
    set_.references = references_;
    set_.capacity = CAPACITY;
    set_.static_count = 1;
    set_.count = 2;
    set_.generation = 1;
    set_.linked = true;
  }

  size_t lend(const Module &module) {
    modules_[1] = module;
    return 1;
  }

  void reopen() { ++set_.generation; }

  const ModuleSet &get() const { return set_; }

private:
  static constexpr size_t CAPACITY = 4;
  ModuleSet set_{};
  Module modules_[CAPACITY];
  MappedModule mappings_[CAPACITY];
  intptr_t offsets_[CAPACITY] = {};
  size_t references_[CAPACITY] = {};
};

} // anonymous namespace

TEST(LlvmLibcElfDtvTest, NothingForAModuleWithoutTls) {
  Record record;
  Module bare;
  const size_t index = record.lend(bare);

  DynamicThreadVector vector;
  EXPECT_EQ(vector.block_for(record.get(), index),
            static_cast<void *>(nullptr));
  vector.release();
}

TEST(LlvmLibcElfDtvTest, NothingForAModuleThatIsNotThere) {
  Record record;
  Module bare;
  record.lend(bare);

  DynamicThreadVector vector;
  EXPECT_EQ(vector.block_for(record.get(), 99), static_cast<void *>(nullptr));
  vector.release();
}

TEST(LlvmLibcElfDtvTest, GivesTheSameBlockBackUntilSomethingChanges) {
  auto loaded = load_module(LIBC_PATH, PAGE);
  ASSERT_TRUE(loaded.has_value());
  LoadedModule c = loaded.value();
  ASSERT_TRUE(c.module.tls() != nullptr);

  Record record;
  const size_t index = record.lend(c.module);

  DynamicThreadVector vector;
  auto *first =
      static_cast<unsigned char *>(vector.block_for(record.get(), index));
  ASSERT_TRUE(first != nullptr);
  // Asking again is the same block: a thread local keeps its value.
  EXPECT_EQ(static_cast<void *>(vector.block_for(record.get(), index)),
            static_cast<void *>(first));

  const unsigned char was = first[0];
  first[0] = static_cast<unsigned char>(was + 1);
  EXPECT_EQ(
      static_cast<unsigned char *>(vector.block_for(record.get(), index))[0],
      static_cast<unsigned char>(was + 1));

  // Once something has been opened or closed, an index may name a different
  // module, so what was kept for it is given up and made again from the
  // module's own image. The kernel may hand back the same memory, so what
  // says this happened is the contents rather than the address.
  record.reopen();
  auto *second =
      static_cast<unsigned char *>(vector.block_for(record.get(), index));
  ASSERT_TRUE(second != nullptr);
  EXPECT_EQ(second[0], was);

  vector.release();
  unmap_module(c.mapping);
}

TEST(LlvmLibcElfDtvTest, BlockStartsFromTheModulesInitialImage) {
  auto loaded = load_module(LIBC_PATH, PAGE);
  ASSERT_TRUE(loaded.has_value());
  LoadedModule c = loaded.value();
  const ElfW(Phdr) *tls = c.module.tls();
  ASSERT_TRUE(tls != nullptr);

  Record record;
  const size_t index = record.lend(c.module);

  DynamicThreadVector vector;
  auto *block =
      static_cast<const unsigned char *>(vector.block_for(record.get(), index));
  ASSERT_TRUE(block != nullptr);

  // The alignment the segment asks for has to be met.
  if (tls->p_align > 1)
    EXPECT_EQ(reinterpret_cast<uintptr_t>(block) % tls->p_align, uintptr_t(0));

  // What was initialised in the module is copied in, and the rest is zero.
  const auto *image = reinterpret_cast<const unsigned char *>(
      c.module.load_bias() + tls->p_vaddr);
  for (size_t i = 0; i < tls->p_filesz; ++i)
    ASSERT_EQ(block[i], image[i]);
  for (size_t i = tls->p_filesz; i < tls->p_memsz; ++i)
    ASSERT_EQ(block[i], static_cast<unsigned char>(0));

  vector.release();
  unmap_module(c.mapping);
}

TEST(LlvmLibcElfDtvTest, EachVectorHasItsOwnBlocks) {
  auto loaded = load_module(LIBC_PATH, PAGE);
  ASSERT_TRUE(loaded.has_value());
  LoadedModule c = loaded.value();
  ASSERT_TRUE(c.module.tls() != nullptr);

  Record record;
  const size_t index = record.lend(c.module);

  // One of these belongs to each thread, so no two hand out the same block.
  DynamicThreadVector one;
  DynamicThreadVector two;
  void *first = one.block_for(record.get(), index);
  void *second = two.block_for(record.get(), index);
  ASSERT_TRUE(first != nullptr);
  ASSERT_TRUE(second != nullptr);
  EXPECT_NE(first, second);

  one.release();
  two.release();
  unmap_module(c.mapping);
}
