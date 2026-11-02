//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Binding PLT slots on first call
///
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
#include "src/__support/elf/dynamic_blocks.h"
#include "src/__support/elf/passive_abi.h"
#include "src/__support/elf/scope.h"
#include "src/__support/elf/thread_pointer.h"
#include "src/__support/macros/config.h"
#include "src/__support/macros/properties/architectures.h"
#include "src/dlfcn/dl_internal.h"
#include "src/unistd/environ.h"

#if defined(LIBC_TARGET_ARCH_IS_X86_64) ||                                     \
    defined(LIBC_TARGET_ARCH_IS_AARCH64) ||                                    \
    defined(LIBC_TARGET_ARCH_IS_ANY_RISCV)

// The size of the register save area the stubs below use, and whether it is
// written with XSAVE or FXSAVE.
extern "C" {
#if defined(LIBC_TARGET_ARCH_IS_X86_64)
// Read only by the stubs below, which the optimiser cannot see into.
[[gnu::visibility("hidden"), gnu::used]] size_t __llvm_libc_dl_state_size = 512;
[[gnu::visibility("hidden"),
  gnu::used]] unsigned char __llvm_libc_dl_use_xsave = 0;
#endif
[[gnu::visibility("hidden")]] ElfW(Addr)
    __llvm_libc_dl_lazy_bind(ElfW(Addr) got, size_t index);
[[gnu::visibility("hidden")]] void __llvm_libc_dl_lazy_entry();
[[gnu::visibility("hidden")]] uintptr_t
__llvm_libc_tlsdesc_dynamic_offset(uintptr_t argument);
[[gnu::visibility("hidden")]] void __llvm_libc_tlsdesc_dynamic();
}

#if defined(LIBC_TARGET_ARCH_IS_X86_64)

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

// The descriptor function for a thread local of a module opened after
// startup. It calls into C to find or make the thread's block, so it saves
// every register but %rax, and the vector state, as the convention requires.
asm(R"(
  .text
  .p2align 4
  .globl __llvm_libc_tlsdesc_dynamic
  .hidden __llvm_libc_tlsdesc_dynamic
  .type __llvm_libc_tlsdesc_dynamic, @function
__llvm_libc_tlsdesc_dynamic:
  endbr64
  pushq %rbx
  movq %rsp, %rbx
  pushq %rdi
  pushq %rsi
  pushq %rdx
  pushq %rcx
  pushq %r8
  pushq %r9
  pushq %r10
  pushq %r11
  pushq $0
  movq 8(%rax), %rdi
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
  call __llvm_libc_tlsdesc_dynamic_offset
  movq %rax, -72(%rbx)
  cmpb $0, __llvm_libc_dl_use_xsave(%rip)
  je 3f
  movl $-1, %eax
  movl $-1, %edx
  xrstor64 (%rsp)
  jmp 4f
3:
  fxrstor64 (%rsp)
4:
  leaq -72(%rbx), %rsp
  popq %rax
  popq %r11
  popq %r10
  popq %r9
  popq %r8
  popq %rcx
  popq %rdx
  popq %rsi
  popq %rdi
  popq %rbx
  ret
  .size __llvm_libc_tlsdesc_dynamic, . - __llvm_libc_tlsdesc_dynamic
)");

#elif defined(LIBC_TARGET_ARCH_IS_AARCH64)

// Reached from PLT0 with x16 holding the address of GOT[2] and x17 this
// function, above the x16 and x30 PLT0 pushed. The first of those is the
// address of the slot being bound. Saves every register a call may pass
// arguments in, binds the slot and jumps to the target.
asm(R"(
  .text
  .p2align 2
  .globl __llvm_libc_dl_lazy_entry
  .hidden __llvm_libc_dl_lazy_entry
  .type __llvm_libc_dl_lazy_entry, %function
__llvm_libc_dl_lazy_entry:
  sub sp, sp, #208
  stp x0, x1, [sp, #0]
  stp x2, x3, [sp, #16]
  stp x4, x5, [sp, #32]
  stp x6, x7, [sp, #48]
  str x8, [sp, #64]
  stp q0, q1, [sp, #80]
  stp q2, q3, [sp, #112]
  stp q4, q5, [sp, #144]
  stp q6, q7, [sp, #176]
  // The GOT starts two words before GOT[2]. The slot's index is its distance
  // from GOT[3], in words.
  sub x0, x16, #16
  ldr x1, [sp, #208]
  sub x1, x1, x0
  sub x1, x1, #24
  lsr x1, x1, #3
  bl __llvm_libc_dl_lazy_bind
  mov x17, x0
  ldp x0, x1, [sp, #0]
  ldp x2, x3, [sp, #16]
  ldp x4, x5, [sp, #32]
  ldp x6, x7, [sp, #48]
  ldr x8, [sp, #64]
  ldp q0, q1, [sp, #80]
  ldp q2, q3, [sp, #112]
  ldp q4, q5, [sp, #144]
  ldp q6, q7, [sp, #176]
  add sp, sp, #208
  ldp x16, x30, [sp], #16
  br x17
  .size __llvm_libc_dl_lazy_entry, . - __llvm_libc_dl_lazy_entry
)");

// The descriptor function for a thread local of a module opened after
// startup. It calls into C to find or make the thread's block, so it saves
// every register but x0 that a call may change, the vector registers C code
// does not preserve included.
asm(R"(
  .text
  .p2align 2
  .globl __llvm_libc_tlsdesc_dynamic
  .hidden __llvm_libc_tlsdesc_dynamic
  .type __llvm_libc_tlsdesc_dynamic, %function
__llvm_libc_tlsdesc_dynamic:
  stp x29, x30, [sp, #-16]!
  mov x29, sp
  sub sp, sp, #528
  stp x1, x2, [sp, #0]
  stp x3, x4, [sp, #16]
  stp x5, x6, [sp, #32]
  stp x7, x8, [sp, #48]
  stp x9, x10, [sp, #64]
  stp x11, x12, [sp, #80]
  stp x13, x14, [sp, #96]
  stp x15, x16, [sp, #112]
  stp x17, x18, [sp, #128]
  stp q0, q1, [sp, #144]
  stp q2, q3, [sp, #176]
  stp q4, q5, [sp, #208]
  stp q6, q7, [sp, #240]
  stp q16, q17, [sp, #272]
  stp q18, q19, [sp, #304]
  stp q20, q21, [sp, #336]
  stp q22, q23, [sp, #368]
  stp q24, q25, [sp, #400]
  stp q26, q27, [sp, #432]
  stp q28, q29, [sp, #464]
  stp q30, q31, [sp, #496]
  ldr x0, [x0, #8]
  bl __llvm_libc_tlsdesc_dynamic_offset
  ldp x1, x2, [sp, #0]
  ldp x3, x4, [sp, #16]
  ldp x5, x6, [sp, #32]
  ldp x7, x8, [sp, #48]
  ldp x9, x10, [sp, #64]
  ldp x11, x12, [sp, #80]
  ldp x13, x14, [sp, #96]
  ldp x15, x16, [sp, #112]
  ldp x17, x18, [sp, #128]
  ldp q0, q1, [sp, #144]
  ldp q2, q3, [sp, #176]
  ldp q4, q5, [sp, #208]
  ldp q6, q7, [sp, #240]
  ldp q16, q17, [sp, #272]
  ldp q18, q19, [sp, #304]
  ldp q20, q21, [sp, #336]
  ldp q22, q23, [sp, #368]
  ldp q24, q25, [sp, #400]
  ldp q26, q27, [sp, #432]
  ldp q28, q29, [sp, #464]
  ldp q30, q31, [sp, #496]
  mov sp, x29
  ldp x29, x30, [sp], #16
  ret
  .size __llvm_libc_tlsdesc_dynamic, . - __llvm_libc_tlsdesc_dynamic
)");

#elif defined(LIBC_TARGET_ARCH_IS_ANY_RISCV)

// Reached from PLT0 with t0 holding GOT[1], which is the GOT's own address
// here, and t1 eight times the relocation index. Saves every register a call
// may pass arguments in, binds the slot and jumps to the target.
asm(R"(
  .text
  .p2align 2
  .globl __llvm_libc_dl_lazy_entry
  .hidden __llvm_libc_dl_lazy_entry
  .type __llvm_libc_dl_lazy_entry, @function
__llvm_libc_dl_lazy_entry:
  addi sp, sp, -144
  sd ra, 0(sp)
  sd a0, 8(sp)
  sd a1, 16(sp)
  sd a2, 24(sp)
  sd a3, 32(sp)
  sd a4, 40(sp)
  sd a5, 48(sp)
  sd a6, 56(sp)
  sd a7, 64(sp)
  fsd fa0, 72(sp)
  fsd fa1, 80(sp)
  fsd fa2, 88(sp)
  fsd fa3, 96(sp)
  fsd fa4, 104(sp)
  fsd fa5, 112(sp)
  fsd fa6, 120(sp)
  fsd fa7, 128(sp)
  mv a0, t0
  srli a1, t1, 3
  call __llvm_libc_dl_lazy_bind
  mv t1, a0
  ld ra, 0(sp)
  ld a0, 8(sp)
  ld a1, 16(sp)
  ld a2, 24(sp)
  ld a3, 32(sp)
  ld a4, 40(sp)
  ld a5, 48(sp)
  ld a6, 56(sp)
  ld a7, 64(sp)
  fld fa0, 72(sp)
  fld fa1, 80(sp)
  fld fa2, 88(sp)
  fld fa3, 96(sp)
  fld fa4, 104(sp)
  fld fa5, 112(sp)
  fld fa6, 120(sp)
  fld fa7, 128(sp)
  addi sp, sp, 144
  jr t1
  .size __llvm_libc_dl_lazy_entry, . - __llvm_libc_dl_lazy_entry
)");

// The descriptor function for a thread local of a module opened after
// startup. It is called with t0 as the link register and may change nothing
// but a0, t0 and t1. It calls into C to find or make the thread's block, so
// it saves every other register a call may change, the floating point
// registers C code does not preserve included.
asm(R"(
  .text
  .p2align 2
  .globl __llvm_libc_tlsdesc_dynamic
  .hidden __llvm_libc_tlsdesc_dynamic
  .type __llvm_libc_tlsdesc_dynamic, @function
__llvm_libc_tlsdesc_dynamic:
  addi sp, sp, -272
  sd ra, 0(sp)
  sd t0, 8(sp)
  sd a1, 16(sp)
  sd a2, 24(sp)
  sd a3, 32(sp)
  sd a4, 40(sp)
  sd a5, 48(sp)
  sd a6, 56(sp)
  sd a7, 64(sp)
  sd t2, 72(sp)
  sd t3, 80(sp)
  sd t4, 88(sp)
  sd t5, 96(sp)
  sd t6, 104(sp)
  fsd ft0, 112(sp)
  fsd ft1, 120(sp)
  fsd ft2, 128(sp)
  fsd ft3, 136(sp)
  fsd ft4, 144(sp)
  fsd ft5, 152(sp)
  fsd ft6, 160(sp)
  fsd ft7, 168(sp)
  fsd fa0, 176(sp)
  fsd fa1, 184(sp)
  fsd fa2, 192(sp)
  fsd fa3, 200(sp)
  fsd fa4, 208(sp)
  fsd fa5, 216(sp)
  fsd fa6, 224(sp)
  fsd fa7, 232(sp)
  fsd ft8, 240(sp)
  fsd ft9, 248(sp)
  fsd ft10, 256(sp)
  fsd ft11, 264(sp)
  ld a0, 8(a0)
  call __llvm_libc_tlsdesc_dynamic_offset
  ld ra, 0(sp)
  ld t0, 8(sp)
  ld a1, 16(sp)
  ld a2, 24(sp)
  ld a3, 32(sp)
  ld a4, 40(sp)
  ld a5, 48(sp)
  ld a6, 56(sp)
  ld a7, 64(sp)
  ld t2, 72(sp)
  ld t3, 80(sp)
  ld t4, 88(sp)
  ld t5, 96(sp)
  ld t6, 104(sp)
  fld ft0, 112(sp)
  fld ft1, 120(sp)
  fld ft2, 128(sp)
  fld ft3, 136(sp)
  fld ft4, 144(sp)
  fld ft5, 152(sp)
  fld ft6, 160(sp)
  fld ft7, 168(sp)
  fld fa0, 176(sp)
  fld fa1, 184(sp)
  fld fa2, 192(sp)
  fld fa3, 200(sp)
  fld fa4, 208(sp)
  fld fa5, 216(sp)
  fld fa6, 224(sp)
  fld fa7, 232(sp)
  fld ft8, 240(sp)
  fld ft9, 248(sp)
  fld ft10, 256(sp)
  fld ft11, 264(sp)
  addi sp, sp, 272
  jr t0
  .size __llvm_libc_tlsdesc_dynamic, . - __llvm_libc_tlsdesc_dynamic
)");

#endif

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

[[noreturn]] void no_tls_block(const char *module) {
  write_to_stderr("cannot allocate thread local storage for ");
  write_to_stderr(module != nullptr ? module : "?");
  write_to_stderr("\n");
  internal::exit(127);
}

} // anonymous namespace
} // namespace LIBC_NAMESPACE_DECL

extern "C" [[gnu::used]] uintptr_t
__llvm_libc_tlsdesc_dynamic_offset(uintptr_t argument) {
  using namespace LIBC_NAMESPACE;
  const size_t module = argument >> elf::TLS_DESCRIPTOR_MODULE_SHIFT;
  const uintptr_t offset = argument & elf::TLS_DESCRIPTOR_OFFSET_MASK;
  void *block = elf::dynamic_block_for(module);
  if (block == nullptr)
    no_tls_block(elf::loaded_modules().modules[module].name());
  return reinterpret_cast<uintptr_t>(block) + offset - elf::thread_pointer();
}

// Called only from the stub above, which the optimiser cannot see into.
extern "C" [[gnu::used]] ElfW(Addr)
    __llvm_libc_dl_lazy_bind(ElfW(Addr) got, size_t index) {
  using namespace LIBC_NAMESPACE;
  // The lock is recursive, so a slot first called from a constructor that
  // dlopen is running, with the lock held, still binds.
  cpp::lock_guard lock(dl::dl_mutex);
  elf::ModuleSet &set = elf::loaded_modules();

  const elf::Module *module = nullptr;
  size_t requester = 0;
  for (size_t i = 0; i < set.count; ++i) {
    if (set.modules[i].contains(got)) {
      module = &set.modules[i];
      requester = i;
      break;
    }
  }
  if (module == nullptr)
    lookup_failed(nullptr, "a PLT slot of a module that is not loaded");

  const elf::RelaTable table = module->plt_relocations();
  if (index >= table.size())
    lookup_failed(module->name(), "a PLT slot past the end of its table");
  const ElfW(Rela) &rela = table.begin()[index];
  const uint32_t symbol_index = elf::reloc_symbol(rela.r_info);
  const char *name = module->strtab() + module->symtab()[symbol_index].st_name;
  const char *version = module->symbol_version(symbol_index);

  // The definition may come from a module opened after this one.
  elf::SearchOrder order(set.modules, set.count);
  bool *hidden = static_cast<bool *>(__builtin_alloca(set.count));
  elf::hide_from(set, requester, hidden);
  order.set_hidden(hidden);
  auto found = order.resolve(name, version);
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

#if defined(LIBC_TARGET_ARCH_IS_X86_64)
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

#else

void measure_saved_state() {}

#endif

} // anonymous namespace

ElfW(Addr) dynamic_tls_descriptor() {
  measure_saved_state();
  return reinterpret_cast<ElfW(Addr)>(&__llvm_libc_tlsdesc_dynamic);
}

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
#if defined(LIBC_TARGET_ARCH_IS_ANY_RISCV)
  // PLT0 jumps through the first word, and the slots start at the third.
  got[0] = reinterpret_cast<ElfW(Addr)>(&__llvm_libc_dl_lazy_entry);
#else
  got[2] = reinterpret_cast<ElfW(Addr)>(&__llvm_libc_dl_lazy_entry);
#endif
}

} // namespace dl
} // namespace LIBC_NAMESPACE_DECL

#else

namespace LIBC_NAMESPACE_DECL {
namespace dl {

ElfW(Addr) dynamic_tls_descriptor() { return 0; }

bool can_bind_lazily(const elf::Module &) { return false; }

void enable_lazy_binding(const elf::Module &) {}

} // namespace dl
} // namespace LIBC_NAMESPACE_DECL

#endif
