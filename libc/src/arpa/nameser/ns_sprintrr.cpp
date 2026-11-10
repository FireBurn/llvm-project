//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of ns_sprintrr
///
//===----------------------------------------------------------------------===//

#include "src/arpa/nameser/ns_sprintrr.h"

#include "hdr/errno_macros.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

namespace {

// Appends to a buffer, and notes it when there is not room.
class Out {
public:
  Out(char *buf, size_t size) : buf(buf), size(size) {}
  void put(char c) {
    if (len + 1 < size)
      buf[len] = c;
    ++len;
  }
  void text(const char *s) {
    for (; *s != '\0'; ++s)
      put(*s);
  }
  void number(unsigned long v) {
    char digits[24];
    int n = 0;
    do {
      digits[n++] = static_cast<char>('0' + v % 10);
      v /= 10;
    } while (v != 0);
    while (n > 0)
      put(digits[--n]);
  }
  void hex(unsigned char b) {
    put("0123456789abcdef"[b >> 4]);
    put("0123456789abcdef"[b & 15]);
  }
  bool fits() const { return len < size; }
  int finish() {
    if (!fits())
      return -1;
    buf[len] = '\0';
    return static_cast<int>(len);
  }

private:
  char *buf;
  size_t size;
  size_t len = 0;
};

} // namespace

// Writes the record as one line: name, TTL, class, type, and the data in the
// generic form RFC 3597 gives, which does not need to know the type. The
// name and origin are not used to shorten the name.
LLVM_LIBC_FUNCTION(int, ns_sprintrr,
                   (const ns_msg *handle, const ns_rr *rr, const char *name_ctx,
                    const char *origin, char *buf, size_t buflen)) {
  (void)handle;
  (void)name_ctx;
  (void)origin;
  if (rr == nullptr || buf == nullptr || buflen == 0) {
    libc_errno = EINVAL;
    return -1;
  }
  Out out(buf, buflen);
  out.text(rr->name[0] != '\0' ? rr->name : ".");
  out.put('\t');
  out.number(rr->ttl);
  out.put('\t');
  switch (rr->rr_class) {
  case 1:
    out.text("IN");
    break;
  case 3:
    out.text("CH");
    break;
  case 4:
    out.text("HS");
    break;
  default:
    out.text("CLASS");
    out.number(rr->rr_class);
  }
  out.put('\t');
  out.text("TYPE");
  out.number(rr->type);
  out.put('\t');
  out.text("\\# ");
  out.number(rr->rdlength);
  for (unsigned i = 0; i < rr->rdlength; ++i) {
    out.put(' ');
    out.hex(rr->rdata[i]);
  }
  const int n = out.finish();
  if (n < 0)
    libc_errno = ENOSPC;
  return n;
}

} // namespace LIBC_NAMESPACE_DECL
