//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the declaration of the loader of service modules.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC___SUPPORT_NSS_MODULE_H
#define LLVM_LIBC_SRC___SUPPORT_NSS_MODULE_H

#include "src/__support/nss/config.h"

namespace LIBC_NAMESPACE_DECL {
namespace nss {

// The function _nss_<service>_<function> of the module libnss_<service>.so.2,
// or nullptr where there is no such module or it lacks the function. The
// module is loaded the first time it is asked for and kept.
void *service_function(Source &source, const char *function);

// Stands in for the module loader, for tests.
using Resolver = void *(*)(const char *service, const char *function);
void TESTONLY_set_resolver(Resolver resolver);

} // namespace nss
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC___SUPPORT_NSS_MODULE_H
