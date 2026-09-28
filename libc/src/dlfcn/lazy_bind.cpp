//===-- Binding PLT slots on first call -----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/dlfcn/lazy_bind.h"

#include "hdr/elf_macros.h"
#include "hdr/elf_proxy.h"
#include "hdr/link_macros.h"
#include "hdr/stdint_proxy.h"
#include "hdr/types/size_t.h"
#include "src/__support/CPP/mutex.h"
#include "src/__support/CPP/string_view.h"
#include "src/__support/OSUtil/exit.h"
#include "src/__support/OSUtil/io.h"
#include "src/__support/elf/bind.h"
#include "src/__support/elf/passive_abi.h"
#include "src/__support/macros/config.h"
#include "src/__support/macros/properties/architectures.h"
#include "src/dlfcn/dl_internal.h"
#include "src/unistd/environ.h"

#if defined(LIBC_TARGET_ARCH_IS_X86_64)

// The size of the register save area the stubs below use, and whether it is
// written with XSAVE or FXSAVE.
extern "C" {
// Read only by the stubs below, which the optimiser cannot see into.
[[gnu::visibility("hidden"), gnu::used]] size_t __llvm_libc_dl_state_size = 512;
[[gnu::visibility("hidden"), gnu::used]] unsigned char __llvm_libc_dl_use_xsave =
    0;
[[gnu::visibility("hidden")]] ElfW(Addr)
    __llvm_libc_dl_lazy_bind(ElfW(Addr) got, size_t index);
[[gnu::visibility("hidden")]] void __llvm_libc_dl_lazy_entry();
}

// Reached from PLT0 with GOT[1] and the relocation index pushed above the
// caller's return address. Saves every register a call may pass arguments
// in, including the vector state, binds the slot and jumps to the target.
asm(R"(
  .text
  .p2align 4
  .globl __llvm_libc_dl_lazy_entry
  .hidden __llvm_libc_dl_lazy_entry
  .type __llvm_libc_dl_lazy_entry, @function
__llvm_libc_dl_lazy_entry:
  endbr64
  pushq %rbx
  movq %rsp, %rbx
  pushq %rax
  pushq %rcx
  pushq %rdx
  pushq %rsi
  pushq %rdi
  pushq %r8
  pushq %r9
  pushq %r10
  andq $-64, %rsp
  subq __llvm_libc_dl_state_size(%rip), %rsp
  cmpb $0, __llvm_libc_dl_use_xsave(%rip)
  je 1f
  xorl %eax, %eax
  movq %rax, 512(%rsp)
  movq %rax, 520(%rsp)
  movq %rax, 528(%rsp)
  movq %rax, 536(%rsp)
  movq %rax, 544(%rsp)
  movq %rax, 552(%rsp)
  movq %rax, 560(%rsp)
  movq %rax, 568(%rsp)
  movl $-1, %eax
  movl $-1, %edx
  xsave64 (%rsp)
  jmp 2f
1:
  fxsave64 (%rsp)
2:
  movq 8(%rbx), %rdi
  movq 16(%rbx), %rsi
  call __llvm_libc_dl_lazy_bind
  movq %rax, %r11
  cmpb $0, __llvm_libc_dl_use_xsave(%rip)
  je 3f
  movl $-1, %eax
  movl $-1, %edx
  xrstor64 (%rsp)
  jmp 4f
3:
  fxrstor64 (%rsp)
4:
  leaq -64(%rbx), %rsp
  popq %r10
  popq %r9
  popq %r8
  popq %rdi
  popq %rsi
  popq %rdx
  popq %rcx
  popq %rax
  popq %rbx
  addq $16, %rsp
  jmp *%r11
  .size __llvm_libc_dl_lazy_entry, . - __llvm_libc_dl_lazy_entry
)");

namespace LIBC_NAMESPACE_DECL {
namespace {

[[noreturn]] void lookup_failed(const char *module, const char *symbol) {
  write_to_stderr("symbol lookup error: ");
  write_to_stderr(module != nullptr ? module : "?");
  write_to_stderr(": undefined symbol: ");
  write_to_stderr(symbol);
  write_to_stderr("\n");
  internal::exit(127);
}

} // anonymous namespace
} // namespace LIBC_NAMESPACE_DECL

// Called only from the stub above, which the optimiser cannot see into.
extern "C" [[gnu::used]] ElfW(Addr)
    __llvm_libc_dl_lazy_bind(ElfW(Addr) got, size_t index) {
  using namespace LIBC_NAMESPACE;
  // The lock is recursive, so a slot first called from a constructor that
  // dlopen is running, with the lock held, still binds.
  cpp::lock_guard lock(dl::dl_mutex);
  elf::ModuleSet &set = elf::loaded_modules();

  const elf::Module *module = nullptr;
  for (size_t i = 0; i < set.count; ++i) {
    if (set.modules[i].contains(got)) {
      module = &set.modules[i];
      break;
    }
  }
  if (module == nullptr)
    lookup_failed(nullptr, "a PLT slot of a module that is not loaded");

  const elf::RelaTable table = module->plt_relocations();
  if (index >= table.size())
    lookup_failed(module->name(), "a PLT slot past the end of its table");
  const ElfW(Rela) &rela = table.begin()[index];
  const char *name = module->strtab() +
                     module->symtab()[elf::reloc_symbol(rela.r_info)].st_name;

  // The definition may come from a module opened after this one.
  elf::SearchOrder order(set.modules, set.count);
  auto found = order.resolve(name);
  if (!found)
    lookup_failed(module->name(), name);

  *reinterpret_cast<ElfW(Addr) *>(module->load_bias() + rela.r_offset) = *found;
  return *found;
}

namespace LIBC_NAMESPACE_DECL {
namespace dl {

namespace {

// LD_BIND_NOW set to anything at all asks for everything to be bound at load
// time, as it does under glibc.
bool bind_now_requested() {
  char **env = reinterpret_cast<char **>(environ);
  if (env == nullptr)
    return false;
  constexpr cpp::string_view NAME = "LD_BIND_NOW=";
  for (; *env != nullptr; ++env) {
    cpp::string_view entry(*env);
    if (entry.starts_with(NAME) && entry.size() > NAME.size())
      return true;
  }
  return false;
}

void cpuid(uint32_t leaf, uint32_t subleaf, uint32_t &a, uint32_t &b,
           uint32_t &c, uint32_t &d) {
  asm volatile("cpuid"
               : "=a"(a), "=b"(b), "=c"(c), "=d"(d)
               : "a"(leaf), "c"(subleaf));
}

// Sizes the save area for the extended state the kernel has enabled, or
// settles for FXSAVE where XSAVE is not available.
void measure_saved_state() {
  static bool measured = false;
  if (measured)
    return;
  measured = true;
  uint32_t a, b, c, d;
  cpuid(1, 0, a, b, c, d);
  constexpr uint32_t OSXSAVE = 1u << 27;
  if ((c & OSXSAVE) == 0)
    return;
  cpuid(0xd, 0, a, b, c, d);
  // The header XSAVE writes sits just past the legacy area.
  if (b < 576)
    return;
  __llvm_libc_dl_state_size = (static_cast<size_t>(b) + 63) & ~size_t(63);
  __llvm_libc_dl_use_xsave = 1;
}

} // anonymous namespace

bool can_bind_lazily(const elf::Module &module) {
  if (elf::binds_now(module) || bind_now_requested())
    return false;
  return module.dynamic().address(DT_PLTGOT).has_value() &&
         !module.plt_relocations().empty();
}

void enable_lazy_binding(const elf::Module &module) {
  measure_saved_state();
  auto *got =
      reinterpret_cast<ElfW(Addr) *>(*module.dynamic().address(DT_PLTGOT));
  // The binder finds the module again from the GOT's address.
  got[1] = reinterpret_cast<ElfW(Addr)>(got);
  got[2] = reinterpret_cast<ElfW(Addr)>(&__llvm_libc_dl_lazy_entry);
}

} // namespace dl
} // namespace LIBC_NAMESPACE_DECL

#else

namespace LIBC_NAMESPACE_DECL {
namespace dl {

bool can_bind_lazily(const elf::Module &) { return false; }

void enable_lazy_binding(const elf::Module &) {}

} // namespace dl
} // namespace LIBC_NAMESPACE_DECL

#endif
