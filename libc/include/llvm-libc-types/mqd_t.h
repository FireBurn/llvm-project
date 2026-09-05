//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of mqd_t type.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_MQD_T_H
#define LLVM_LIBC_TYPES_MQD_T_H

// What mq_open hands back. On Linux a queue is a file, so this is a
// descriptor.
typedef int mqd_t;

#endif // LLVM_LIBC_TYPES_MQD_T_H
