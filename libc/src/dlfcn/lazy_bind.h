//===-- Binding PLT slots on first call -------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_DLFCN_LAZY_BIND_H
#define LLVM_LIBC_SRC_DLFCN_LAZY_BIND_H

#include "src/__support/elf/module.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {
namespace dl {

// Whether the module's PLT slots can be left for their first call to bind:
// the target has a resolver, the module was not linked to be bound at load
// time, and it has a GOT to point at the resolver.
bool can_bind_lazily(const elf::Module &module);

// Points the module's PLT at the resolver, for the slots bind_module left
// deferred.
void enable_lazy_binding(const elf::Module &module);

} // namespace dl
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_DLFCN_LAZY_BIND_H
