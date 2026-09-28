//===-- Unittests for the debugger's list of modules ----------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/link_macros.h"
#include "src/__support/elf/load_module.h"
#include "src/link/link_maps.h"
#include "test/UnitTest/Test.h"

using LIBC_NAMESPACE::elf::load_module;
using LIBC_NAMESPACE::elf::Module;
using LIBC_NAMESPACE::elf::ModuleSet;
using LIBC_NAMESPACE::elf::unmap_module;
using LIBC_NAMESPACE::link::build_link_maps;

namespace {
constexpr size_t PAGE = 4096;
constexpr const char *LIBC_PATH = LIBC_TEST_SHARED_OBJECT;
constexpr const char *LIBM_PATH = LIBM_TEST_SHARED_OBJECT;
} // anonymous namespace

TEST(LlvmLibcLinkMapsTest, ChainsEveryModule) {
  auto libc = load_module(LIBC_PATH, PAGE);
  ASSERT_TRUE(libc.has_value());
  auto libm = load_module(LIBM_PATH, PAGE);
  ASSERT_TRUE(libm.has_value());

  Module modules[2] = {libc.value().module, libm.value().module};
  ModuleSet set = {};
  set.modules = modules;
  set.count = 2;

  // A chain the startup code set up statically is replaced, not freed.
  struct link_map fixed = {};
  struct r_debug debug = {};
  debug.r_map = &fixed;
  ASSERT_TRUE(build_link_maps(set, debug, 0));
  ASSERT_TRUE(debug.r_map != &fixed);

  struct link_map *first = debug.r_map;
  EXPECT_TRUE(first->l_prev == nullptr);
  EXPECT_STREQ(first->l_name, "");
  EXPECT_EQ(first->l_addr, modules[0].load_bias());
  EXPECT_TRUE(first->l_ld == modules[0].dynamic().entries());

  struct link_map *second = first->l_next;
  ASSERT_TRUE(second != nullptr);
  EXPECT_TRUE(second->l_prev == first);
  EXPECT_STREQ(second->l_name, LIBM_PATH);
  EXPECT_EQ(second->l_addr, modules[1].load_bias());
  EXPECT_TRUE(second->l_ld == modules[1].dynamic().entries());
  EXPECT_TRUE(second->l_next == nullptr);

  // Dropping a module shortens the chain, and the old one is released.
  set.count = 1;
  ASSERT_TRUE(build_link_maps(set, debug, 0));
  EXPECT_TRUE(debug.r_map->l_next == nullptr);

  // The loader goes last, found from its base address.
  const ElfW(Addr) base = libm.value().module.load_bias();
  ASSERT_TRUE(build_link_maps(set, debug, base));
  struct link_map *loader = debug.r_map->l_next;
  ASSERT_TRUE(loader != nullptr);
  EXPECT_EQ(loader->l_addr, base);
  EXPECT_TRUE(loader->l_ld == modules[1].dynamic().entries());
  EXPECT_TRUE(loader->l_next == nullptr);

  unmap_module(libm.value().mapping);
  unmap_module(libc.value().mapping);
}
