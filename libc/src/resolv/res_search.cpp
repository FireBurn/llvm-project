//===-- Implementation of res_search --------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/resolv/res_search.h"

#include "hdr/netdb_macros.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/netdb/__h_errno_location.h"
#include "src/netdb/resolv/dns_message.h"
#include "src/resolv/res_query.h"
#include "src/resolv/res_state.h"
#include "src/string/memory_utils/inline_memcpy.h"

namespace LIBC_NAMESPACE_DECL {

namespace {

size_t length_of(const char *text) {
  size_t n = 0;
  while (text[n] != '\0')
    ++n;
  return n;
}

size_t dots_in(const char *name) {
  size_t n = 0;
  for (size_t i = 0; name[i] != '\0'; ++i)
    if (name[i] == '.')
      ++n;
  return n;
}

// Writes |name| followed by a dot and |domain| into |out|.
bool join(const char *name, const char *domain, char *out, size_t capacity) {
  const size_t first = length_of(name);
  const size_t second = length_of(domain);
  if (first + 1 + second + 1 > capacity)
    return false;
  inline_memcpy(out, name, first);
  out[first] = '.';
  inline_memcpy(out + first + 1, domain, second);
  out[first + 1 + second] = '\0';
  return true;
}

} // anonymous namespace

// Asks about a name, trying it in the search domains where it is not already
// qualified. A name with at least ndots dots in it, or one written with a
// trailing dot, is asked about as it stands first, since it is already meant
// to be complete.
LLVM_LIBC_FUNCTION(int, res_search,
                   (const char *name, int rr_class, int type,
                    unsigned char *answer, int anslen)) {
  int *h_errno_location = LIBC_NAMESPACE::__h_errno_location();
  if (name == nullptr || answer == nullptr || anslen <= 0) {
    *h_errno_location = NO_RECOVERY;
    return -1;
  }

  struct __res_state *state = LIBC_NAMESPACE::__res_state();
  internal::res_ready(*state);

  const size_t length = length_of(name);
  const bool rooted = length > 0 && name[length - 1] == '.';
  const bool qualified = rooted || dots_in(name) >= state->ndots;

  if (qualified) {
    const int taken =
        LIBC_NAMESPACE::res_query(name, rr_class, type, answer, anslen);
    if (taken >= 0)
      return taken;
    // A name written with a trailing dot names itself and nothing else, so
    // there is nowhere else to look.
    if (rooted)
      return -1;
  }

  if ((state->options & RES_DNSRCH) != 0) {
    int saved = *h_errno_location;
    for (size_t i = 0; i < MAXDNSRCH && state->dnsrch[i] != nullptr; ++i) {
      char tried[resolv::MAX_MESSAGE];
      if (!join(name, state->dnsrch[i], tried, sizeof(tried)))
        continue;
      const int taken =
          LIBC_NAMESPACE::res_query(tried, rr_class, type, answer, anslen);
      if (taken >= 0)
        return taken;
      // The first refusal is the one worth reporting: a later domain saying
      // it has never heard of the name says less than the first one did.
      if (*h_errno_location == HOST_NOT_FOUND && saved != 0)
        *h_errno_location = saved;
      else
        saved = *h_errno_location;
    }
  }

  if (!qualified) {
    const int taken =
        LIBC_NAMESPACE::res_query(name, rr_class, type, answer, anslen);
    if (taken >= 0)
      return taken;
  }
  return -1;
}

} // namespace LIBC_NAMESPACE_DECL
