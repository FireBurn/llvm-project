//===-- The zone the machine keeps time in --------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/time/tz/timezone.h"

#include "src/__support/CPP/mutex.h"
#include "src/__support/macros/config.h"
#include "src/__support/threads/mutex.h"
#include "src/string/memory_utils/inline_memcpy.h"
#include "src/time/time_constants.h"
#include "src/time/time_utils.h"
#include "src/time/tz/tzfile.h"
#include "src/time/tz/zone_source.h"
#include "src/time/tz_variables.h"

namespace LIBC_NAMESPACE_DECL {
namespace tz {

namespace {

constexpr const char *LOCALTIME_PATH = "/etc/localtime";
constexpr const char *ZONEINFO_DIR = "/usr/share/zoneinfo/";

// How the zone is being told, once tzset has worked it out.
enum class Kind {
  // Greenwich, which is what an unset or empty TZ and an unreadable file all
  // come to.
  UTC,
  // TZ stated the rules itself.
  Rules,
  // TZ named a file, or none was named and /etc/localtime was read.
  File
};

struct State {
  Kind kind = Kind::UTC;
  PosixZone rules;
  TZFile file;
  // What tzset reports for a file, which is not what any one moment in it
  // reads: it is taken from the changes closest to the end.
  TZFile::NamePair file_pair = {};
  // The offset the last moment turned into local time had, kept only as the
  // first guess for the next one. Where a stated time reads two ways, it is
  // what settles which.
  int32_t last_offset = 0;
  // The bytes the file was read as, kept for as long as the reader points
  // into them.
  Mapping mapping;
  bool loaded = false;
  // The names tzname points at, held here because the ones a file gives sit in
  // the mapping and go away when the zone changes.
  char names[2][MAX_ABBREVIATION + 1] = {};
  // What TZ said when this was worked out, so a second tzset with the same
  // setting need not read the file again.
  char setting[256] = {};
  bool had_setting = false;
};

State state;
Mutex state_lock(false, false, false, false);

size_t string_length(const char *text) {
  size_t n = 0;
  while (text[n] != '\0')
    ++n;
  return n;
}

bool same_string(const char *left, const char *right) {
  while (*left != '\0' && *left == *right) {
    ++left;
    ++right;
  }
  return *left == *right;
}

void release_mapping() { unmap_zone_file(state.mapping); }

// Reads a zone out of a file.
bool map_file(const char *path) {
  Mapping mapping;
  if (!map_zone_file(path, mapping))
    return false;

  TZFile parsed;
  if (!parsed.parse(static_cast<const unsigned char *>(mapping.data),
                    mapping.size)) {
    unmap_zone_file(mapping);
    return false;
  }

  release_mapping();
  state.mapping = mapping;
  state.file = parsed;
  state.file_pair = parsed.name_pair();
  state.kind = Kind::File;
  return true;
}

bool map_named_zone(const char *name) {
  // A leading colon says the rest names a file, which it may do without one
  // as well.
  if (*name == ':')
    ++name;
  if (*name == '\0')
    return false;
  if (*name == '/')
    return map_file(name);

  // A name may not climb out of the directory the zones are kept in.
  for (const char *at = name; *at != '\0'; ++at)
    if (at[0] == '.' && at[1] == '.')
      return false;

  char path[256];
  const size_t prefix = string_length(ZONEINFO_DIR);
  const size_t length = string_length(name);
  if (prefix + length + 1 > sizeof(path))
    return false;
  inline_memcpy(path, ZONEINFO_DIR, prefix);
  inline_memcpy(path + prefix, name, length + 1);
  return map_file(path);
}

void copy_name(char *into, const char *from) {
  size_t n = 0;
  while (from[n] != '\0' && n < MAX_ABBREVIATION) {
    into[n] = from[n];
    ++n;
  }
  into[n] = '\0';
}

// Fills in tzname, timezone and daylight, which POSIX says tzset leaves
// describing the zone. timezone is the standard time offset counted west,
// and daylight says only whether the zone ever changes.
void publish() {
  copy_name(state.names[0], "UTC");
  copy_name(state.names[1], "UTC");
  int32_t standard = 0;
  bool ever_daylight = false;

  switch (state.kind) {
  case Kind::UTC:
    break;
  case Kind::Rules:
    copy_name(state.names[0], state.rules.standard_name);
    copy_name(state.names[1], state.rules.daylight_name);
    standard = state.rules.standard_offset;
    ever_daylight = state.rules.has_daylight;
    break;
  case Kind::File:
    copy_name(state.names[0], state.file_pair.standard);
    copy_name(state.names[1], state.file_pair.daylight);
    standard = state.file_pair.standard_offset;
    ever_daylight = state.file_pair.has_daylight;
    break;
  }

  tzname[0] = state.names[0];
  tzname[1] = state.names[1];
  timezone = -static_cast<long>(standard);
  daylight = ever_daylight ? 1 : 0;
}

void load_locked() {
  const char *setting = tz_from_environment();

  // Reading the same setting twice need not read the file twice, which keeps
  // a program that calls tzset around every conversion from touching the disk
  // each time.
  if (state.loaded) {
    const bool have = setting != nullptr;
    if (have == state.had_setting &&
        (!have || same_string(setting, state.setting)))
      return;
  }

  state.had_setting = setting != nullptr;
  state.setting[0] = '\0';
  if (setting != nullptr) {
    size_t n = 0;
    while (setting[n] != '\0' && n < sizeof(state.setting) - 1) {
      state.setting[n] = setting[n];
      ++n;
    }
    state.setting[n] = '\0';
  }

  state.kind = Kind::UTC;
  if (setting == nullptr) {
    // Nothing said, so whatever the machine was set to.
    if (!map_file(LOCALTIME_PATH))
      release_mapping();
  } else if (*setting == '\0') {
    // TZ set to nothing means Greenwich.
    release_mapping();
  } else if (!map_named_zone(setting)) {
    // Nothing readable goes by that name, so it is read as rules instead.
    // A name that states no rules either still stands as the name of a zone
    // at Greenwich rather than being thrown out.
    release_mapping();
    if (parse_posix_tz(setting, state.rules))
      state.kind = Kind::Rules;
  }

  state.loaded = true;
  publish();
}

} // anonymous namespace

void set_from_environment() {
  cpp::lock_guard guard(state_lock);
  load_locked();
}

void ensure_loaded() {
  cpp::lock_guard guard(state_lock);
  if (!state.loaded)
    load_locked();
}

namespace {

Answer answer_locked(time_t seconds, bool republish) {
  switch (state.kind) {
  case Kind::Rules:
    return tz::answer_for(state.rules, seconds);
  case Kind::File: {
    const TZFile::Reading reading = state.file.read_at(seconds);
    if (republish) {
      if (reading.from_footer) {
        // Past the last change listed, the TZ string the file ends with is in
        // charge and states the names as well as the readings.
        copy_name(state.names[0], state.file.footer_rules().standard_name);
        copy_name(state.names[1], state.file.footer_rules().daylight_name);
        timezone =
            -static_cast<long>(state.file.footer_rules().standard_offset);
        daylight = state.file.footer_rules().has_daylight ? 1 : 0;
      } else {
        const char *standard = reading.standard_name;
        const char *dst = reading.daylight_name;
        if (standard == nullptr)
          standard = dst;
        if (dst == nullptr)
          dst = standard;
        if (standard != nullptr)
          copy_name(state.names[0], standard);
        if (dst != nullptr)
          copy_name(state.names[1], dst);
        timezone = -static_cast<long>(state.file_pair.standard_offset);
        daylight = state.file_pair.has_daylight ? 1 : 0;
      }
      tzname[0] = state.names[0];
      tzname[1] = state.names[1];
    }
    return reading.answer;
  }
  case Kind::UTC:
    break;
  }
  return {0, "UTC", false};
}

} // anonymous namespace

Answer answer_for_time(time_t seconds) {
  cpp::lock_guard guard(state_lock);
  if (!state.loaded)
    load_locked();
  return answer_locked(seconds, /*republish=*/false);
}

// The reading for a moment, restating what the zone goes by as it goes.
Answer answer_and_publish(time_t seconds) {
  cpp::lock_guard guard(state_lock);
  if (!state.loaded)
    load_locked();
  return answer_locked(seconds, /*republish=*/true);
}

ErrorOr<tm *> to_local(time_t seconds, tm *result) {
  // POSIX has telling the local time behave as though tzset had been called,
  // so a program that changes TZ and converts without saying so again gets
  // the zone it asked for. Reading it again costs a look at the environment;
  // the file is only read again if what it says has changed.
  set_from_environment();
  const Answer answer = answer_and_publish(seconds);
  auto status =
      time_utils::update_from_seconds(seconds + answer.offset, result);
  if (!status)
    return cpp::unexpected(status.error());
  result->tm_isdst = answer.daylight ? 1 : 0;
#ifdef __linux__
  result->tm_gmtoff = answer.offset;
  result->tm_zone = answer.name;
#endif
  return result;
}

ErrorOr<time_t> from_local_tm(tm *value) {
  set_from_environment();

  // The fields are read as a count of seconds as if the zone were Greenwich,
  // which is exactly the local time the caller stated.
  auto as_utc = time_utils::mktime_internal(value);
  if (!as_utc)
    return cpp::unexpected(time_utils::TIME_OVERFLOW);

  int32_t offset;
  const char *name;
  bool daylight;
  const time_t seconds =
      from_local(*as_utc, value->tm_isdst, offset, &name, daylight);

  // mktime leaves the struct saying the moment it settled on in normal form,
  // which is what the zone reads there rather than what the caller wrote.
  auto normalized = to_local(seconds, value);
  if (!normalized)
    return cpp::unexpected(normalized.error());
  return seconds;
}

namespace {

int32_t guess_offset() {
  cpp::lock_guard guard(state_lock);
  return state.last_offset;
}

void remember_offset(int32_t offset) {
  cpp::lock_guard guard(state_lock);
  state.last_offset = offset;
}

} // anonymous namespace

time_t from_local(time_t local_seconds, int daylight_hint, int32_t &offset,
                  const char **name, bool &daylight) {
  // The offset to take off depends on the moment, and the moment depends on
  // the offset. The offset the last conversion used is tried first and the
  // error it leaves used to improve on it, which settles in one step for a
  // time that reads only one way.
  time_t candidate = local_seconds - guess_offset();
  time_t previous = candidate;
  time_t before_that = candidate;
  bool previous_daylight = false;
  Answer settled = answer_for_time(candidate);

  // Six tries is more than any zone needs and stops a bad one looping.
  for (int probes = 6; probes > 0; --probes) {
    settled = answer_for_time(candidate);
    const time_t error = local_seconds - (candidate + settled.offset);
    if (error == 0)
      break;

    // Two offsets the stated time falls between, with neither reading it
    // back. It lies in the gap a change forward skips over, which no moment
    // maps onto. The reading whose daylight flag is not the one asked for is
    // taken, which is what everything else does and is more use than failing.
    if (candidate == before_that && candidate != previous &&
        (daylight_hint < 0 ? previous_daylight <= settled.daylight
                           : (daylight_hint != 0) != settled.daylight))
      break;

    before_that = previous;
    previous = candidate;
    previous_daylight = settled.daylight;
    candidate += error;
  }

  // Where the caller stated which of the two readings it meant, honour it.
  // The zone is searched for a moment on the other reading and its offset
  // used, so a stated time either side of a change comes out as meant.
  if (daylight_hint >= 0 && settled.daylight != (daylight_hint != 0)) {
    const bool wanted = daylight_hint != 0;
    // Even steps out from the moment, short enough not to step over the
    // briefest spell either way round that the database records, and going
    // out far enough to reach the longest.
    constexpr time_t STEP = 601200;
    constexpr time_t BOUND = 457243209 / 2 + STEP;
    bool found = false;
    for (time_t away = STEP; away < BOUND && !found; away += STEP) {
      for (int sign = -1; sign <= 1 && !found; sign += 2) {
        const Answer other = answer_for_time(candidate + sign * away);
        if (other.daylight == wanted) {
          candidate = local_seconds - other.offset;
          settled = answer_for_time(candidate);
          found = true;
        }
      }
    }
    if (!found) {
      // The zone never reads the way the caller asked. An hour is assumed,
      // which is what every zone that changes at all moves by.
      candidate += settled.daylight ? 3600 : -3600;
      settled = answer_for_time(candidate);
    }
  }

  // What the conversion came to, which is the offset it settled on except
  // where the stated time had no moment of its own and it had to be moved.
  remember_offset(static_cast<int32_t>(local_seconds - candidate));
  offset = settled.offset;
  *name = settled.name;
  daylight = settled.daylight;
  return candidate;
}

} // namespace tz
} // namespace LIBC_NAMESPACE_DECL
