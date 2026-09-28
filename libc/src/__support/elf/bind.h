//===-- Binding a module's symbol relocations -------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC___SUPPORT_ELF_BIND_H
#define LLVM_LIBC_SRC___SUPPORT_ELF_BIND_H

#include "hdr/elf_macros.h"
#include "hdr/elf_proxy.h"
#include "hdr/link_macros.h"
#include "hdr/stdint_proxy.h"
#include "hdr/types/size_t.h"
#include "src/__support/CPP/optional.h"
#include "src/__support/elf/module.h"
#include "src/__support/elf/relocation.h"
#include "src/__support/elf/tlsdesc.h"
#include "src/__support/macros/attributes.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {
namespace elf {

// Relocation types that take a symbol value. Anything else is either relative,
// handled before this point, or unsupported.
#if defined(LIBC_TARGET_ARCH_IS_X86_64)
constexpr uint32_t RELOC_ABSOLUTE = R_X86_64_64;
constexpr uint32_t RELOC_COPY = R_X86_64_COPY;
constexpr uint32_t RELOC_GLOB_DAT = R_X86_64_GLOB_DAT;
constexpr uint32_t RELOC_JUMP_SLOT = R_X86_64_JUMP_SLOT;
constexpr uint32_t RELOC_TLS_OFFSET = R_X86_64_TPOFF64;
// The general dynamic model, which a shared library uses because it does not
// know at link time where in the thread's blocks its own will sit. The pair
// is read at run time by __tls_get_addr.
constexpr uint32_t RELOC_TLS_MODULE = R_X86_64_DTPMOD64;
constexpr uint32_t RELOC_TLS_MODULE_OFFSET = R_X86_64_DTPOFF64;
// A descriptor: a function and an argument that between them give the
// offset of a thread local from the thread pointer.
constexpr uint32_t RELOC_TLS_DESCRIPTOR = R_X86_64_TLSDESC;
#elif defined(LIBC_TARGET_ARCH_IS_AARCH64)
constexpr uint32_t RELOC_ABSOLUTE = R_AARCH64_ABS64;
constexpr uint32_t RELOC_COPY = R_AARCH64_COPY;
constexpr uint32_t RELOC_GLOB_DAT = R_AARCH64_GLOB_DAT;
constexpr uint32_t RELOC_JUMP_SLOT = R_AARCH64_JUMP_SLOT;
constexpr uint32_t RELOC_TLS_OFFSET = R_AARCH64_TLS_TPREL64;
constexpr uint32_t RELOC_TLS_MODULE = R_AARCH64_TLS_DTPMOD;
constexpr uint32_t RELOC_TLS_MODULE_OFFSET = R_AARCH64_TLS_DTPREL;
constexpr uint32_t RELOC_TLS_DESCRIPTOR = 0xfffffffcu;
#elif defined(LIBC_TARGET_ARCH_IS_ANY_RISCV)
constexpr uint32_t RELOC_ABSOLUTE = R_RISCV_64;
constexpr uint32_t RELOC_COPY = R_RISCV_COPY;
constexpr uint32_t RELOC_GLOB_DAT = R_RISCV_JUMP_SLOT;
constexpr uint32_t RELOC_JUMP_SLOT = R_RISCV_JUMP_SLOT;
constexpr uint32_t RELOC_TLS_OFFSET = R_RISCV_TLS_TPREL64;
constexpr uint32_t RELOC_TLS_MODULE = R_RISCV_TLS_DTPMOD64;
constexpr uint32_t RELOC_TLS_MODULE_OFFSET = R_RISCV_TLS_DTPREL64;
constexpr uint32_t RELOC_TLS_DESCRIPTOR = 0xfffffffcu;
#elif defined(LIBC_TARGET_ARCH_IS_ARM)
constexpr uint32_t RELOC_ABSOLUTE = R_ARM_ABS32;
constexpr uint32_t RELOC_COPY = R_ARM_COPY;
constexpr uint32_t RELOC_GLOB_DAT = R_ARM_GLOB_DAT;
constexpr uint32_t RELOC_JUMP_SLOT = R_ARM_JUMP_SLOT;
// ARM uses a different TLS model; not handled here yet.
constexpr uint32_t RELOC_TLS_OFFSET = 0xffffffffu;
constexpr uint32_t RELOC_TLS_MODULE = 0xfffffffeu;
constexpr uint32_t RELOC_TLS_MODULE_OFFSET = 0xfffffffdu;
constexpr uint32_t RELOC_TLS_DESCRIPTOR = 0xfffffffcu;
#else
#error "Symbol relocation types are not known for this architecture"
#endif

LIBC_INLINE constexpr unsigned char symbol_binding(unsigned char info) {
#ifdef __LP64__
  return static_cast<unsigned char>(ELF64_ST_BIND(info));
#else
  return static_cast<unsigned char>(ELF32_ST_BIND(info));
#endif
}

LIBC_INLINE constexpr uint32_t reloc_symbol(ElfW(Xword) info) {
#ifdef __LP64__
  return static_cast<uint32_t>(ELF64_R_SYM(info));
#else
  return static_cast<uint32_t>(ELF32_R_SYM(info));
#endif
}

// An ordered set of modules, searched in load order the way a dynamic loader
// does. The first definition found wins, which is what makes interposition
// work.
class SearchOrder {
public:
  LIBC_INLINE constexpr SearchOrder(const Module *modules, size_t count)
      : modules_(modules), count_(count) {}

  struct Resolved {
    ElfW(Addr) address;
    // Which module defined it, so a thread local can be placed relative to
    // that module's block rather than this one's.
    size_t module;
    ElfW(Addr) value;
  };

  // `first` skips the modules before it, which is what a copy relocation
  // needs: the executable reserved the space and must not satisfy its own
  // lookup with it.
  LIBC_INLINE cpp::optional<Resolved> find(const char *name,
                                           size_t first = 0) const {
    for (size_t i = first; i < count_; ++i) {
      const ElfW(Sym) *symbol = modules_[i].symbols().lookup(name);
      if (symbol == nullptr)
        continue;
      // A symbol the module only references rather than defines has no
      // section, and must not satisfy the lookup.
      if (symbol->st_shndx == SHN_UNDEF)
        continue;
      return Resolved{modules_[i].load_bias() + symbol->st_value, i,
                      symbol->st_value};
    }
    return cpp::nullopt;
  }

  // Returns the runtime address of `name`, or nothing if no module defines it.
  LIBC_INLINE cpp::optional<ElfW(Addr)> resolve(const char *name) const {
    if (auto found = find(name))
      return found->address;
    return cpp::nullopt;
  }

  // Offsets of each module's thread local block from the thread pointer, as
  // the loader laid them out. Needed to place a thread local symbol.
  LIBC_INLINE void set_tls_offsets(const intptr_t *offsets) {
    tls_offsets_ = offsets;
  }
  LIBC_INLINE const intptr_t *tls_offsets() const { return tls_offsets_; }

  // How many modules, from the first, have a place in every thread's static
  // block. Those opened later have a block per thread made on demand, and a
  // descriptor for one of their thread locals needs `dynamic_descriptor`,
  // which only libc can supply.
  LIBC_INLINE void set_static_tls(size_t static_count,
                                  ElfW(Addr) dynamic_descriptor) {
    static_count_ = static_count;
    dynamic_descriptor_ = dynamic_descriptor;
  }
  LIBC_INLINE size_t static_count() const { return static_count_; }
  LIBC_INLINE ElfW(Addr) dynamic_descriptor() const {
    return dynamic_descriptor_;
  }

private:
  const Module *modules_;
  size_t count_;
  const intptr_t *tls_offsets_ = nullptr;
  size_t static_count_ = static_cast<size_t>(-1);
  ElfW(Addr) dynamic_descriptor_ = 0;
};

// A dynamic descriptor's argument packs the module index and the offset.
constexpr unsigned TLS_DESCRIPTOR_MODULE_SHIFT = 48;
constexpr ElfW(Addr) TLS_DESCRIPTOR_OFFSET_MASK =
    (ElfW(Addr)(1) << TLS_DESCRIPTOR_MODULE_SHIFT) - 1;

struct BindResult {
  size_t bound = 0;
  size_t unresolved = 0;
  // PLT slots left pointing at their lazy stubs, to be bound on first call.
  size_t deferred = 0;
  // The first symbol that could not be found, so that whoever gives up can
  // say which one it was. A count on its own leaves the reader to work that
  // out with a disassembler.
  const char *missing = nullptr;
};

// Applies one table of symbol relocations against the search order.
//
// A strong symbol that cannot be found is counted rather than fatal, so the
// caller decides whether to fail the load. With `lazy`, a PLT slot whose
// symbol cannot be found yet is left pointing at its stub instead, so that it
// is looked up again when it is first called.
LIBC_INLINE BindResult bind_relocations(const Module &target,
                                        size_t target_index,
                                        const RelaTable &table,
                                        const SearchOrder &order,
                                        bool lazy = false) {
  BindResult result;
  const ElfW(Sym) *symtab = target.symtab();
  const char *strtab = target.strtab();
  if (symtab == nullptr || strtab == nullptr)
    return result;

  for (const ElfW(Rela) *rela = table.begin(); rela != table.end(); ++rela) {
    const uint32_t type = reloc_type(rela->r_info);
    const bool is_tls = type == RELOC_TLS_OFFSET;
    const bool is_tls_module = type == RELOC_TLS_MODULE;
    const bool is_tls_module_offset = type == RELOC_TLS_MODULE_OFFSET;
    const bool is_copy = type == RELOC_COPY;
    const bool is_tls_descriptor = type == RELOC_TLS_DESCRIPTOR;
    if (type != RELOC_ABSOLUTE && type != RELOC_GLOB_DAT &&
        type != RELOC_JUMP_SLOT && !is_copy && !is_tls && !is_tls_module &&
        !is_tls_module_offset && !is_tls_descriptor)
      continue;

    const uint32_t symbol_index = reloc_symbol(rela->r_info);
    const ElfW(Sym) &symbol = symtab[symbol_index];
    const char *name = strtab + symbol.st_name;

    if (is_tls_module || is_tls_module_offset) {
      // The general dynamic model. What is stored is not an address but a
      // module and an offset within that module's block, which
      // __tls_get_addr turns into an address once it knows which thread is
      // asking.
      size_t module = target_index;
      ElfW(Addr) value = 0;
      if (symbol_index != 0 && symbol.st_shndx == SHN_UNDEF) {
        auto defined = order.find(name);
        if (!defined) {
          ++result.unresolved;
          if (result.missing == nullptr)
            result.missing = name;
          continue;
        }
        module = defined->module;
        value = defined->value;
      } else if (symbol_index != 0) {
        value = symbol.st_value;
      }
      auto *slot =
          reinterpret_cast<ElfW(Addr) *>(target.load_bias() + rela->r_offset);
      *slot = is_tls_module ? static_cast<ElfW(Addr)>(module)
                            : static_cast<ElfW(Addr)>(value + rela->r_addend);
      ++result.bound;
      continue;
    }

    if (is_tls_descriptor) {
      size_t module = target_index;
      ElfW(Addr) value = 0;
      if (symbol_index != 0 && symbol.st_shndx == SHN_UNDEF) {
        auto defined = order.find(name);
        if (!defined) {
          ++result.unresolved;
          if (result.missing == nullptr)
            result.missing = name;
          continue;
        }
        module = defined->module;
        value = defined->value;
      } else if (symbol_index != 0) {
        value = symbol.st_value;
      }
      const ElfW(Addr) offset = value + static_cast<ElfW(Addr)>(rela->r_addend);
      auto *descriptor =
          reinterpret_cast<ElfW(Addr) *>(target.load_bias() + rela->r_offset);
      const intptr_t *offsets = order.tls_offsets();
#if defined(LIBC_TARGET_ARCH_IS_X86_64)
      if (module < order.static_count() && offsets != nullptr) {
        descriptor[0] =
            reinterpret_cast<ElfW(Addr)>(&__llvm_libc_tlsdesc_static);
        descriptor[1] = static_cast<ElfW(Addr)>(offsets[module]) + offset;
        ++result.bound;
        continue;
      }
      if (order.dynamic_descriptor() != 0 &&
          offset <= TLS_DESCRIPTOR_OFFSET_MASK &&
          module < (size_t(1) << (64 - TLS_DESCRIPTOR_MODULE_SHIFT))) {
        descriptor[0] = order.dynamic_descriptor();
        descriptor[1] =
            (static_cast<ElfW(Addr)>(module) << TLS_DESCRIPTOR_MODULE_SHIFT) |
            offset;
        ++result.bound;
        continue;
      }
#endif
      (void)descriptor;
      (void)offsets;
      ++result.unresolved;
      if (result.missing == nullptr)
        result.missing = name;
      continue;
    }

    if (is_tls) {
      // A thread local is addressed relative to the thread pointer, so what
      // gets stored is the defining module's block offset plus the symbol's
      // offset inside it, never an absolute address.
      const intptr_t *offsets = order.tls_offsets();
      if (offsets == nullptr) {
        ++result.unresolved;
        if (result.missing == nullptr)
          result.missing = name;
        continue;
      }
      intptr_t block = 0;
      ElfW(Addr) value = 0;
      if (symbol_index == 0 || symbol.st_shndx != SHN_UNDEF) {
        // Either no symbol at all, where the addend alone gives the offset
        // within this module's block, or one defined here. Most of a libc's
        // own thread locals are hidden, so they are in no hash table and can
        // only be resolved here rather than through the search order.
        block = offsets[target_index];
        value = symbol_index == 0 ? 0 : symbol.st_value;
      } else if (auto defined = order.find(name)) {
        block = offsets[defined->module];
        value = defined->value;
      } else {
        ++result.unresolved;
        if (result.missing == nullptr)
          result.missing = name;
        continue;
      }
      auto *slot =
          reinterpret_cast<ElfW(Addr) *>(target.load_bias() + rela->r_offset);
      *slot = static_cast<ElfW(Addr)>(block + static_cast<intptr_t>(value) +
                                      rela->r_addend);
      ++result.bound;
      continue;
    }

    if (is_copy) {
      // An executable that is not position independent reserves space for a
      // data symbol it imports and takes a copy of it, so that its own
      // references can be plain absolute addresses. The definition has to
      // come from somewhere other than the executable itself.
      auto defined = order.find(name, target_index + 1);
      if (!defined) {
        ++result.unresolved;
        if (result.missing == nullptr)
          result.missing = name;
        continue;
      }
      auto *dest = reinterpret_cast<unsigned char *>(target.load_bias() +
                                                     rela->r_offset);
      const auto *src =
          reinterpret_cast<const unsigned char *>(defined->address);
      for (size_t i = 0; i < symbol.st_size; ++i)
        dest[i] = src[i];
      ++result.bound;
      continue;
    }

    auto found = order.resolve(name);

    // A weak undefined symbol is allowed to stay unresolved and is written as
    // zero, which is what lets a module test for an optional dependency.
    ElfW(Addr) value = 0;
    if (found) {
      value = *found;
    } else if (lazy && type == RELOC_JUMP_SLOT) {
      // The linker left the slot holding the link time address of the
      // instruction after the stub's first jump, which is where it pushes
      // the relocation's index and enters the resolver.
      auto *where =
          reinterpret_cast<ElfW(Addr) *>(target.load_bias() + rela->r_offset);
      *where += target.load_bias();
      ++result.deferred;
      continue;
    } else if (symbol_binding(symbol.st_info) != STB_WEAK) {
      ++result.unresolved;
      if (result.missing == nullptr)
        result.missing = name;
      continue;
    }

    auto *where =
        reinterpret_cast<ElfW(Addr) *>(target.load_bias() + rela->r_offset);
    // Only the absolute form carries an addend that matters here; the GOT and
    // PLT slots take the symbol value on its own.
    *where = (type == RELOC_ABSOLUTE)
                 ? value + static_cast<ElfW(Addr)>(rela->r_addend)
                 : value;
    ++result.bound;
  }
  return result;
}

// Binds both a module's ordinary and PLT relocations. Only the PLT ones can
// be deferred.
LIBC_INLINE BindResult bind_module(const Module &target, size_t target_index,
                                   const SearchOrder &order,
                                   bool lazy = false) {
  BindResult a =
      bind_relocations(target, target_index, target.relocations(), order);
  BindResult b = bind_relocations(target, target_index,
                                  target.plt_relocations(), order, lazy);
  return BindResult{a.bound + b.bound, a.unresolved + b.unresolved, b.deferred,
                    a.missing != nullptr ? a.missing : b.missing};
}

// Whether the module was linked to have everything bound at load time, which
// rules out deferring any of it.
LIBC_INLINE bool binds_now(const Module &module) {
  const DynamicTable &dynamic = module.dynamic();
  if (dynamic.contains(DT_BIND_NOW))
    return true;
  if (auto flags = dynamic.value(DT_FLAGS); flags && (*flags & DF_BIND_NOW))
    return true;
  if (auto flags = dynamic.value(DT_FLAGS_1); flags && (*flags & DF_1_NOW))
    return true;
  return false;
}

} // namespace elf
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC___SUPPORT_ELF_BIND_H
