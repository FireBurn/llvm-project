//===-- Building and reading a name server's messages -----------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_NETDB_RESOLV_DNS_MESSAGE_H
#define LLVM_LIBC_SRC_NETDB_RESOLV_DNS_MESSAGE_H

#include "hdr/stdint_proxy.h"
#include "hdr/types/size_t.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {
namespace resolv {

// The record types this asks for. There is no need for the rest.
constexpr uint16_t TYPE_A = 1;
constexpr uint16_t TYPE_CNAME = 5;
constexpr uint16_t TYPE_PTR = 12;
constexpr uint16_t TYPE_AAAA = 28;
constexpr uint16_t CLASS_IN = 1;

// A question and its answer are both at most this long over datagrams, which
// is what the protocol allowed before anything was added to it.
constexpr size_t MAX_MESSAGE = 512;
// A header is twelve bytes: an identifier, the flags, and four counts.
constexpr size_t HEADER_SIZE = 12;

// What a name server said about a question it was asked.
enum class Status {
  Ok,
  // The name does not exist. Asking again, or asking somebody else, will not
  // change that.
  NoName,
  // Something went wrong at the server, or the message made no sense. Another
  // server may do better.
  Failed
};

// Writes a query for `name` of type `type` into `out`, and returns how long
// it is, or zero if the name will not fit or is not a name.
size_t build_query(const char *name, uint16_t type, uint16_t id,
                   unsigned char *out, size_t capacity);

// What one record in an answer holds, for the types this asks about.
struct Record {
  uint16_t type;
  // Where the data sits in the message, and how long it is.
  const unsigned char *data;
  size_t length;
};

// Reads the name at `at`, following the pointers a message may use to avoid
// repeating one, and writes it into `out` in the form people write. Returns
// how many bytes of the message the name took where it was read, or zero if
// it is malformed.
size_t read_name(const unsigned char *message, size_t length, size_t at,
                 char *out, size_t capacity);

// Steps over the name at `at` without reading it, returning where what
// follows begins, or zero if it is malformed.
size_t skip_name(const unsigned char *message, size_t length, size_t at);

// Calls `callback(record)` for each answer in `message`. Returns what the
// server said about the question.
//
// The callback is given a record whose data still points into the message, so
// it has to copy anything it wants to keep, and is given the message too so
// it can read a name the record points into.
template <typename F>
Status for_each_answer(const unsigned char *message, size_t length,
                       uint16_t expected_id, F callback) {
  if (length < HEADER_SIZE)
    return Status::Failed;
  const uint16_t id = static_cast<uint16_t>(message[0] << 8) | message[1];
  if (id != expected_id)
    return Status::Failed;
  // The message has to be an answer, and not a truncated one: what is missing
  // from a truncated answer may be the part that was wanted.
  if ((message[2] & 0x80) == 0 || (message[2] & 0x02) != 0)
    return Status::Failed;
  const unsigned char code = message[3] & 0x0F;
  if (code == 3)
    return Status::NoName;
  if (code != 0)
    return Status::Failed;

  const size_t questions = (static_cast<size_t>(message[4]) << 8) | message[5];
  const size_t answers = (static_cast<size_t>(message[6]) << 8) | message[7];

  size_t at = HEADER_SIZE;
  for (size_t i = 0; i < questions; ++i) {
    at = skip_name(message, length, at);
    if (at == 0 || at + 4 > length)
      return Status::Failed;
    at += 4;
  }

  for (size_t i = 0; i < answers; ++i) {
    at = skip_name(message, length, at);
    // The type, the class, how long it may be kept, and the length.
    if (at == 0 || at + 10 > length)
      return Status::Failed;
    const uint16_t type =
        static_cast<uint16_t>(message[at] << 8) | message[at + 1];
    const uint16_t klass =
        static_cast<uint16_t>(message[at + 2] << 8) | message[at + 3];
    const size_t data_length =
        (static_cast<size_t>(message[at + 8]) << 8) | message[at + 9];
    at += 10;
    if (at + data_length > length)
      return Status::Failed;
    if (klass == CLASS_IN)
      callback(Record{type, message + at, data_length});
    at += data_length;
  }
  return Status::Ok;
}

} // namespace resolv
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_NETDB_RESOLV_DNS_MESSAGE_H
