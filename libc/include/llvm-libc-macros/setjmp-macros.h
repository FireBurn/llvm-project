//===-- Macros defined in setjmp.h header file ----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_MACROS_SETJMP_MACROS_H
#define LLVM_LIBC_MACROS_SETJMP_MACROS_H

// On Linux neither setjmp nor longjmp touches the signal mask, which is all
// that separates them from the underscored names elsewhere.
#ifndef _setjmp
#define _setjmp setjmp
#endif
#ifndef _longjmp
#define _longjmp longjmp
#endif

#endif // LLVM_LIBC_MACROS_SETJMP_MACROS_H
