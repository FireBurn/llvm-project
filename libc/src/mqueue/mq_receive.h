//===-- Implementation header for mq_receive --------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_MQUEUE_MQ_RECEIVE_H
#define LLVM_LIBC_SRC_MQUEUE_MQ_RECEIVE_H

#include "hdr/types/mqd_t.h"
#include "hdr/types/size_t.h"
#include "hdr/types/ssize_t.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

ssize_t mq_receive(mqd_t mqdes, char *msg_ptr, size_t msg_len,
                   unsigned int *msg_prio);

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_MQUEUE_MQ_RECEIVE_H
