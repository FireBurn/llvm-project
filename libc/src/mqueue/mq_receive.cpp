//===-- Implementation of mq_receive --------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/mqueue/mq_receive.h"

#include "hdr/types/mqd_t.h"
#include "hdr/types/size_t.h"
#include "hdr/types/ssize_t.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/mqueue/mq_timedreceive.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(ssize_t, mq_receive,
                   (mqd_t mqdes, char *msg_ptr, size_t msg_len,
                    unsigned int *msg_prio)) {
  return LIBC_NAMESPACE::mq_timedreceive(mqdes, msg_ptr, msg_len, msg_prio,
                                         nullptr);
}

} // namespace LIBC_NAMESPACE_DECL
