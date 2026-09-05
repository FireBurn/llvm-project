//===-- Reading a TZif file -------------------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_TIME_TZ_TZFILE_H
#define LLVM_LIBC_SRC_TIME_TZ_TZFILE_H

#include "hdr/stdint_proxy.h"
#include "hdr/types/size_t.h"
#include "hdr/types/time_t.h"
#include "src/__support/macros/attributes.h"
#include "src/__support/macros/config.h"
#include "src/time/tz/posix_tz.h"

namespace LIBC_NAMESPACE_DECL {
namespace tz {

// One of the ways a zone tells the time, as a TZif file records it.
struct LocalTimeType {
  int32_t offset;
  bool daylight;
  uint8_t name_index;
};

// A TZif file, read in place out of the bytes it was stored as. Nothing here
// owns those bytes; they have to outlive the reader.
//
// The format is the one RFC 8536 describes. A version 2 or later file holds
// the whole table twice, once with times four bytes wide and again with them
// eight, followed by a TZ string covering the times past the last change it
// lists. Only the second copy and that string are read.
class TZFile {
public:
  // Reads the header and finds the parts. Returns false if the bytes are not
  // a TZif file or do not hold together.
  bool parse(const unsigned char *data, size_t size);

  // What the zone was doing at the given moment, counted from the epoch in
  // UT, together with the pair of names to report for it. The pair is not the
  // one the file as a whole goes by: it is taken from the changes either side
  // of the moment, so a zone reports the names it was using at the time
  // rather than the ones it uses now.
  struct Reading {
    Answer answer;
    const char *standard_name;
    const char *daylight_name;
    // Whether the moment lies past the last change the file lists, where the
    // TZ string it ends with says what happens and states its own names.
    bool from_footer;
  };

  Reading read_at(time_t seconds) const;

  // The rules the file ends with, which only mean anything where read_at said
  // it used them.
  LIBC_INLINE const PosixZone &footer_rules() const { return footer; }

  // The pair of names the zone goes by, which is what tzname reports. Taken
  // by walking the changes backwards from the last and keeping the first name
  // seen for each of the two readings, so a zone that has stopped changing
  // still reports the name it used to change to.
  struct NamePair {
    const char *standard;
    const char *daylight;
    int32_t standard_offset;
    bool has_daylight;
  };

  NamePair name_pair() const;

private:
  Answer answer_from_type(size_t index) const;

  const unsigned char *transitions = nullptr;
  const unsigned char *transition_types = nullptr;
  const unsigned char *types = nullptr;
  const char *names = nullptr;
  size_t transition_count = 0;
  size_t type_count = 0;
  size_t name_bytes = 0;
  // Whether times are eight bytes wide rather than four.
  bool wide_times = false;
  bool has_footer = false;
  PosixZone footer;
};

} // namespace tz
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_TIME_TZ_TZFILE_H
