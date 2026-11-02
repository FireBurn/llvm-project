//===-- An object whose thread local is reached by a descriptor ------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Built with -mtls-dialect=gnu2 (-enable-tlsdesc on aarch64), so it reaches its thread local through a
/// TLS descriptor and not through __tls_get_addr. See tlsdesc_test.cpp.
///
//===----------------------------------------------------------------------===//

__thread int llvm_libc_tlsdesc_value = 7;

extern "C" int *llvm_libc_tlsdesc_address() { return &llvm_libc_tlsdesc_value; }
