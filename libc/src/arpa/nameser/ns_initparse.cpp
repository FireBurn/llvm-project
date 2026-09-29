//===-- Implementation of ns_initparse ------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/arpa/nameser/ns_initparse.h"

#include "hdr/errno_macros.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/arpa/nameser/ns_parse_state.h"
#include "src/arpa/nameser/ns_skiprr.h"

namespace LIBC_NAMESPACE_DECL {

// Reads the header and finds where each section starts. The sections have to
// account for the whole message.
LLVM_LIBC_FUNCTION(int, ns_initparse,
                   (const unsigned char *msg, int msglen, ns_msg *handle)) {
  if (msg == nullptr || handle == nullptr || msglen < 12) {
    libc_errno = EMSGSIZE;
    return -1;
  }
  const unsigned char *eom = msg + msglen;
  handle->_msg = msg;
  handle->_eom = eom;
  handle->_id = static_cast<uint16_t>((msg[0] << 8) | msg[1]);
  handle->_flags = static_cast<uint16_t>((msg[2] << 8) | msg[3]);
  const unsigned char *at = msg + 4;
  for (int i = 0; i < nameser::SECTIONS; ++i, at += 2)
    handle->_counts[i] = static_cast<uint16_t>((at[0] << 8) | at[1]);
  for (int i = 0; i < nameser::SECTIONS; ++i) {
    handle->_sections[i] = handle->_counts[i] == 0 ? nullptr : at;
    int used = ns_skiprr(at, eom, i, handle->_counts[i]);
    if (used < 0)
      return -1;
    at += used;
  }
  if (at != eom) {
    libc_errno = EMSGSIZE;
    return -1;
  }
  nameser::set_section(*handle, nameser::SECTIONS);
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL
