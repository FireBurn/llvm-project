//===-- Implementation of ns_parserr --------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/arpa/nameser/ns_parserr.h"

#include "hdr/errno_macros.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/arpa/nameser/ns_name_uncompress.h"
#include "src/arpa/nameser/ns_parse_state.h"
#include "src/arpa/nameser/ns_skiprr.h"

namespace LIBC_NAMESPACE_DECL {

// Reads record `rrnum` of `section`, or the next one where `rrnum` is -1.
// Reading the records in order does not go back over the ones before.
LLVM_LIBC_FUNCTION(int, ns_parserr,
                   (ns_msg * handle, int section, int rrnum, ns_rr *rr)) {
  if (section < 0 || section >= nameser::SECTIONS) {
    libc_errno = ENODEV;
    return -1;
  }
  if (section != handle->_sect)
    nameser::set_section(*handle, section);
  if (rrnum == -1)
    rrnum = handle->_rrnum;
  if (rrnum < 0 || rrnum >= handle->_counts[section]) {
    libc_errno = ENODEV;
    return -1;
  }
  if (rrnum < handle->_rrnum)
    nameser::set_section(*handle, section);
  if (rrnum > handle->_rrnum) {
    int used = ns_skiprr(handle->_msg_ptr, handle->_eom, section,
                         rrnum - handle->_rrnum);
    if (used < 0)
      return -1;
    handle->_msg_ptr += used;
    handle->_rrnum = rrnum;
  }

  int used = ns_name_uncompress(handle->_msg, handle->_eom, handle->_msg_ptr,
                                rr->name, sizeof(rr->name));
  if (used < 0)
    return -1;
  const unsigned char *at = handle->_msg_ptr + used;
  const unsigned char *eom = handle->_eom;
  if (eom - at < 4) {
    libc_errno = EMSGSIZE;
    return -1;
  }
  rr->type = static_cast<uint16_t>((at[0] << 8) | at[1]);
  rr->rr_class = static_cast<uint16_t>((at[2] << 8) | at[3]);
  at += 4;
  if (section == 0) {
    rr->ttl = 0;
    rr->rdlength = 0;
    rr->rdata = nullptr;
  } else {
    if (eom - at < 6) {
      libc_errno = EMSGSIZE;
      return -1;
    }
    rr->ttl = (static_cast<uint32_t>(at[0]) << 24) |
              (static_cast<uint32_t>(at[1]) << 16) |
              (static_cast<uint32_t>(at[2]) << 8) | at[3];
    rr->rdlength = static_cast<uint16_t>((at[4] << 8) | at[5]);
    at += 6;
    if (eom - at < rr->rdlength) {
      libc_errno = EMSGSIZE;
      return -1;
    }
    rr->rdata = at;
    at += rr->rdlength;
  }
  handle->_msg_ptr = at;
  if (++handle->_rrnum > handle->_counts[section])
    nameser::set_section(*handle, section + 1);
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL
