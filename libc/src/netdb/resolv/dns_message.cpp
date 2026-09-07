//===-- Building and reading a name server's messages ---------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/netdb/resolv/dns_message.h"

#include "src/__support/macros/config.h"
#include "src/string/string_utils.h"

namespace LIBC_NAMESPACE_DECL {
namespace resolv {

size_t build_query(const char *name, uint16_t type, uint16_t rr_class,
                   uint16_t id, unsigned char *out, size_t capacity) {
  if (name == nullptr || out == nullptr)
    return 0;
  const size_t name_length = internal::string_length(name);
  if (name_length == 0 || name_length > 253)
    return 0;
  // A header, the name as its labels, then the type and the class.
  if (capacity < HEADER_SIZE + name_length + 2 + 4)
    return 0;

  out[0] = static_cast<unsigned char>(id >> 8);
  out[1] = static_cast<unsigned char>(id);
  // One question, asked of a server that should ask others on our behalf.
  out[2] = 0x01;
  out[3] = 0x00;
  out[4] = 0x00;
  out[5] = 0x01;
  for (size_t i = 6; i < HEADER_SIZE; ++i)
    out[i] = 0;

  // Each part of the name is written with its length in front of it, and a
  // zero length ends the name.
  size_t at = HEADER_SIZE;
  size_t start = 0;
  for (size_t i = 0;; ++i) {
    if (name[i] != '.' && name[i] != '\0')
      continue;
    const size_t label = i - start;
    // A part of a name is at most sixty three bytes, and only the very last
    // one may be empty, which is how a name that ends in a dot is written.
    if (label > 63)
      return 0;
    if (label == 0) {
      if (name[i] == '\0' && i > 0)
        break;
      return 0;
    }
    out[at++] = static_cast<unsigned char>(label);
    for (size_t j = 0; j < label; ++j)
      out[at++] = static_cast<unsigned char>(name[start + j]);
    if (name[i] == '\0')
      break;
    start = i + 1;
  }
  out[at++] = 0;

  out[at++] = static_cast<unsigned char>(type >> 8);
  out[at++] = static_cast<unsigned char>(type);
  out[at++] = static_cast<unsigned char>(rr_class >> 8);
  out[at++] = static_cast<unsigned char>(rr_class);
  return at;
}

size_t read_name(const unsigned char *message, size_t length, size_t at,
                 char *out, size_t capacity) {
  size_t written = 0;
  size_t here = at;
  size_t consumed = 0;
  // A name may point back into the message rather than repeat what is there,
  // and a message that points to itself would go round for ever, so only as
  // many steps as there are bytes are allowed.
  size_t steps = 0;

  for (;;) {
    if (here >= length || ++steps > length)
      return 0;
    const unsigned char label = message[here];
    if ((label & 0xC0) == 0xC0) {
      // Two bytes naming somewhere earlier in the message.
      if (here + 1 >= length)
        return 0;
      const size_t target =
          (static_cast<size_t>(label & 0x3F) << 8) | message[here + 1];
      if (consumed == 0)
        consumed = here + 2 - at;
      if (target >= here)
        return 0; // Only backwards, so it must end.
      here = target;
      continue;
    }
    if ((label & 0xC0) != 0)
      return 0; // The other two forms were never defined.
    if (label == 0) {
      if (consumed == 0)
        consumed = here + 1 - at;
      break;
    }
    if (here + 1 + label > length)
      return 0;
    if (written != 0) {
      if (written + 1 >= capacity)
        return 0;
      out[written++] = '.';
    }
    if (written + label >= capacity)
      return 0;
    for (size_t i = 0; i < label; ++i)
      out[written++] = static_cast<char>(message[here + 1 + i]);
    here += 1 + label;
  }

  out[written] = '\0';
  return consumed;
}

size_t skip_name(const unsigned char *message, size_t length, size_t at) {
  size_t here = at;
  size_t steps = 0;
  for (;;) {
    if (here >= length || ++steps > length)
      return 0;
    const unsigned char label = message[here];
    if ((label & 0xC0) == 0xC0)
      // A pointer is the whole of what is left of the name.
      return here + 2 <= length ? here + 2 : 0;
    if ((label & 0xC0) != 0)
      return 0;
    if (label == 0)
      return here + 1;
    here += 1 + label;
  }
}

} // namespace resolv
} // namespace LIBC_NAMESPACE_DECL
