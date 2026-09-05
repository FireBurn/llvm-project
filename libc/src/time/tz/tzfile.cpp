//===-- Reading a TZif file -----------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/time/tz/tzfile.h"

#include "src/__support/macros/config.h"
#include "src/string/memory_utils/inline_memcpy.h"

namespace LIBC_NAMESPACE_DECL {
namespace tz {

namespace {

// Every number in the file is stored big endian.
uint32_t read_u32(const unsigned char *at) {
  return (static_cast<uint32_t>(at[0]) << 24) |
         (static_cast<uint32_t>(at[1]) << 16) |
         (static_cast<uint32_t>(at[2]) << 8) | static_cast<uint32_t>(at[3]);
}

int64_t read_i64(const unsigned char *at) {
  uint64_t value = 0;
  for (int i = 0; i < 8; ++i)
    value = (value << 8) | at[i];
  return static_cast<int64_t>(value);
}

constexpr size_t HEADER_SIZE = 44;
constexpr size_t TYPE_SIZE = 6;

// The six counts a header ends with.
struct Header {
  size_t is_ut;
  size_t is_std;
  size_t leaps;
  size_t transitions;
  size_t types;
  size_t names;
};

bool read_header(const unsigned char *data, size_t size, Header &out) {
  if (size < HEADER_SIZE)
    return false;
  if (data[0] != 'T' || data[1] != 'Z' || data[2] != 'i' || data[3] != 'f')
    return false;
  out.is_ut = read_u32(data + 20);
  out.is_std = read_u32(data + 24);
  out.leaps = read_u32(data + 28);
  out.transitions = read_u32(data + 32);
  out.types = read_u32(data + 36);
  out.names = read_u32(data + 40);
  return true;
}

// How many bytes the block following a header takes up.
bool block_size(const Header &header, size_t time_size, size_t &out) {
  // Held to a size the arithmetic below cannot run past. No real file comes
  // anywhere near it.
  constexpr size_t LIMIT = 1u << 24;
  if (header.transitions > LIMIT || header.types > LIMIT ||
      header.names > LIMIT || header.leaps > LIMIT || header.is_std > LIMIT ||
      header.is_ut > LIMIT)
    return false;
  out = header.transitions * (time_size + 1) + header.types * TYPE_SIZE +
        header.names + header.leaps * (time_size + 4) + header.is_std +
        header.is_ut;
  return true;
}

} // anonymous namespace

bool TZFile::parse(const unsigned char *data, size_t size) {
  Header header;
  if (!read_header(data, size, header))
    return false;

  const unsigned char version = data[4];
  const unsigned char *block = data + HEADER_SIZE;
  size_t remaining = size - HEADER_SIZE;
  size_t time_size = 4;

  if (version != '\0') {
    // Step over the four byte wide copy and read the header of the wide one.
    size_t first;
    if (!block_size(header, 4, first) || first > remaining)
      return false;
    block += first;
    remaining -= first;
    if (!read_header(block, remaining, header))
      return false;
    block += HEADER_SIZE;
    remaining -= HEADER_SIZE;
    time_size = 8;
  }

  size_t needed;
  if (!block_size(header, time_size, needed) || needed > remaining)
    return false;
  // A file with no way to tell the time is no use.
  if (header.types == 0 || header.names == 0)
    return false;

  wide_times = time_size == 8;
  transition_count = header.transitions;
  type_count = header.types;
  name_bytes = header.names;
  transitions = block;
  transition_types = transitions + transition_count * time_size;
  types = transition_types + transition_count;
  names = reinterpret_cast<const char *>(types + type_count * TYPE_SIZE);
  // The names have to end in a terminator, or reading one runs off the end.
  if (names[name_bytes - 1] != '\0')
    return false;

  // Every transition has to name a type that is there.
  for (size_t i = 0; i < transition_count; ++i)
    if (transition_types[i] >= type_count)
      return false;
  // As does every type a name.
  for (size_t i = 0; i < type_count; ++i)
    if (types[i * TYPE_SIZE + 5] >= name_bytes)
      return false;

  has_footer = false;
  if (version != '\0') {
    // What is left is the TZ string, held between two newlines.
    const char *at = reinterpret_cast<const char *>(block + needed);
    const char *end = reinterpret_cast<const char *>(data + size);
    if (at < end && *at == '\n') {
      ++at;
      const char *stop = at;
      while (stop < end && *stop != '\n')
        ++stop;
      char text[128];
      const size_t length = static_cast<size_t>(stop - at);
      if (stop < end && length < sizeof(text)) {
        inline_memcpy(text, at, length);
        text[length] = '\0';
        has_footer = parse_posix_tz(text, footer);
      }
    }
  }
  return true;
}

Answer TZFile::answer_from_type(size_t index) const {
  const unsigned char *type = types + index * TYPE_SIZE;
  Answer answer;
  answer.offset = static_cast<int32_t>(read_u32(type));
  answer.daylight = type[4] != 0;
  answer.name = names + type[5];
  return answer;
}

TZFile::NamePair TZFile::name_pair() const {
  NamePair pair{nullptr, nullptr, 0, false};
  for (size_t i = transition_count; i > 0;) {
    const size_t index = transition_types[--i];
    const bool daylight = types[index * TYPE_SIZE + 4] != 0;
    if (daylight) {
      if (pair.daylight == nullptr) {
        pair.daylight = names + types[index * TYPE_SIZE + 5];
        pair.has_daylight = true;
      }
    } else if (pair.standard == nullptr) {
      pair.standard = names + types[index * TYPE_SIZE + 5];
      pair.standard_offset =
          static_cast<int32_t>(read_u32(types + index * TYPE_SIZE));
    }
    if (pair.standard != nullptr && pair.daylight != nullptr)
      break;
  }

  if (pair.standard == nullptr) {
    // A zone that never changed states its one reading as a type with no
    // change pointing at it.
    const Answer only = answer_from_type(0);
    pair.standard = only.name;
    pair.standard_offset = only.offset;
  }
  if (pair.daylight == nullptr)
    pair.daylight = pair.standard;
  return pair;
}

TZFile::Reading TZFile::read_at(time_t seconds) const {
  const auto transition_at = [&](size_t i) -> int64_t {
    if (wide_times)
      return read_i64(transitions + i * 8);
    return static_cast<int32_t>(read_u32(transitions + i * 4));
  };
  const auto name_of = [&](size_t index) -> const char * {
    return names + types[index * TYPE_SIZE + 5];
  };
  const auto is_daylight = [&](size_t index) -> bool {
    return types[index * TYPE_SIZE + 4] != 0;
  };

  Reading reading{{}, nullptr, nullptr, false};

  if (transition_count != 0 &&
      static_cast<int64_t>(seconds) >= transition_at(transition_count - 1) &&
      has_footer) {
    reading.answer = tz::answer_for(footer, seconds);
    reading.from_footer = true;
    return reading;
  }

  size_t chosen;
  if (transition_count == 0 ||
      static_cast<int64_t>(seconds) < transition_at(0)) {
    // Before the first change, the first type that is not daylight time,
    // with any daylight one passed on the way kept as the other name.
    size_t i = 0;
    while (i < type_count && is_daylight(i)) {
      if (reading.daylight_name == nullptr)
        reading.daylight_name = name_of(i);
      ++i;
    }
    if (i == type_count)
      i = 0;
    chosen = i;
    reading.standard_name = name_of(i);
    if (reading.daylight_name == nullptr)
      for (size_t j = i; j < type_count; ++j)
        if (is_daylight(j)) {
          reading.daylight_name = name_of(j);
          break;
        }
  } else {
    // The last change at or before the moment asked about.
    size_t low = 0;
    size_t high = transition_count;
    while (high - low > 1) {
      const size_t mid = low + (high - low) / 2;
      if (transition_at(mid) <= static_cast<int64_t>(seconds))
        low = mid;
      else
        high = mid;
    }
    chosen = transition_types[low];
    if (is_daylight(chosen))
      reading.daylight_name = name_of(chosen);
    else
      reading.standard_name = name_of(chosen);

    // The other name comes from the next change that reads the other way.
    for (size_t j = low + 1; j < transition_count; ++j) {
      const size_t type = transition_types[j];
      if (is_daylight(type)) {
        if (reading.daylight_name == nullptr) {
          reading.daylight_name = name_of(type);
          break;
        }
      } else if (reading.standard_name == nullptr) {
        reading.standard_name = name_of(type);
        break;
      }
    }
    if (reading.standard_name == nullptr)
      reading.standard_name = reading.daylight_name;
  }

  reading.answer = answer_from_type(chosen);
  return reading;
}

} // namespace tz
} // namespace LIBC_NAMESPACE_DECL
