//===-- Implementation of vsyslog -----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/syslog/vsyslog.h"

#include "hdr/fcntl_macros.h"
#include "hdr/syslog_macros.h"
#include "hdr/types/struct_tm.h"
#include "hdr/types/time_t.h"
#include "src/__support/CPP/mutex.h"
#include "src/__support/CPP/string_view.h"
#include "src/__support/OSUtil/syscall.h"
#include "src/__support/StringUtil/error_to_string.h"
#include "src/__support/arg_list.h"
#include "src/__support/common.h"
#include "src/__support/integer_to_string.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/__support/printf_core/printf_main.h"
#include "src/__support/printf_core/writer.h"
#include "src/errno/program_invocation_short_name.h"
#include "src/syslog/syslog_state.h"
#include "src/time/time_utils.h"

#include <sys/socket.h>
#include <sys/syscall.h>

namespace LIBC_NAMESPACE_DECL {

namespace {

// The whole record, header and message together. A syslog datagram is not
// allowed to be split, so anything past this is dropped rather than being
// sent as a second record which would read as a separate line.
constexpr size_t RECORD_MAX = 2048;

// The format after any %m in it has been replaced. A format longer than this
// is used as it stands, which only costs the caller the substitution.
constexpr size_t FORMAT_MAX = 1024;

class Buffer {
  char *data;
  size_t cap;
  size_t len = 0;

public:
  LIBC_INLINE Buffer(char *d, size_t c) : data(d), cap(c) {}

  LIBC_INLINE size_t size() const { return len; }
  LIBC_INLINE char *end() { return data + len; }
  LIBC_INLINE size_t room() const { return cap > len ? cap - len : 0; }

  LIBC_INLINE void put(char c) {
    if (len < cap)
      data[len++] = c;
  }

  LIBC_INLINE void put(cpp::string_view s) {
    for (size_t i = 0; i < s.size(); ++i)
      put(s[i]);
  }

  LIBC_INLINE void put(const char *s) {
    for (; s != nullptr && *s != '\0'; ++s)
      put(*s);
  }

  LIBC_INLINE void put_int(int v) {
    const IntegerToString<int> str(v);
    put(str.view());
  }

  // Two digits, space padded, which is how the day of the month goes out.
  LIBC_INLINE void put_day(int v) {
    put(v < 10 ? ' ' : static_cast<char>('0' + v / 10));
    put(static_cast<char>('0' + v % 10));
  }

  // Two digits, zero padded, for the time of day.
  LIBC_INLINE void put_two(int v) {
    put(static_cast<char>('0' + v / 10));
    put(static_cast<char>('0' + v % 10));
  }
};

// The month names the timestamp uses. This part of the record is fixed by
// the log protocol, so it does not follow the locale.
constexpr const char *MONTHS[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                                    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

void put_timestamp(Buffer &out) {
  time_t now = 0;
  LIBC_NAMESPACE::syscall_impl<long>(SYS_clock_gettime, 0, &now);

  // A struct tm broken out of the seconds. The log wants local time, but the
  // offset is not something the libc knows without reading the zone, so this
  // uses what time_utils works out.
  struct tm tm_storage;
  auto broken_out = time_utils::localtime_internal(&now, &tm_storage);
  if (!broken_out.has_value()) {
    out.put("Jan  1 00:00:00 ");
    return;
  }

  int mon = tm_storage.tm_mon;
  out.put(mon >= 0 && mon < 12 ? MONTHS[mon] : "???");
  out.put(' ');
  out.put_day(tm_storage.tm_mday);
  out.put(' ');
  out.put_two(tm_storage.tm_hour);
  out.put(':');
  out.put_two(tm_storage.tm_min);
  out.put(':');
  out.put_two(tm_storage.tm_sec);
  out.put(' ');
}

// Replaces %m with the message for the current errno, as every syslog does.
// Returns the format to use, which is |scratch| if anything was replaced.
const char *expand_errno(const char *format, char *scratch, size_t cap) {
  bool has_m = false;
  for (const char *p = format; p[0] != '\0'; ++p) {
    if (p[0] == '%' && p[1] == 'm') {
      has_m = true;
      break;
    }
    if (p[0] == '%' && p[1] != '\0')
      ++p;
  }
  if (!has_m)
    return format;

  cpp::string_view msg = get_error_string(libc_errno);
  Buffer out(scratch, cap - 1);
  for (const char *p = format; p[0] != '\0'; ++p) {
    if (p[0] == '%' && p[1] == 'm') {
      // The message goes in as text, so a per cent sign in it has to be
      // doubled or the formatter would read it as a conversion.
      for (size_t i = 0; i < msg.size(); ++i) {
        if (msg[i] == '%')
          out.put('%');
        out.put(msg[i]);
      }
      ++p;
      continue;
    }
    out.put(p[0]);
    if (p[0] == '%' && p[1] != '\0') {
      out.put(p[1]);
      ++p;
    }
  }
  if (out.size() >= cap - 1)
    return format; // It did not fit, so send the format as it came.
  scratch[out.size()] = '\0';
  return scratch;
}

void send_record(const char *record, size_t len, size_t header_len,
                 int option) {
  syslog_internal::State &s = syslog_internal::log_state;

  if (option & LOG_PERROR) {
    LIBC_NAMESPACE::syscall_impl<long>(SYS_write, 2, record + header_len,
                                       len - header_len);
    LIBC_NAMESPACE::syscall_impl<long>(SYS_write, 2, "\n", 1);
  }

  bool sent = false;
  for (int attempt = 0; attempt < 2 && !sent; ++attempt) {
    if (!syslog_internal::connect_unlocked())
      break;
    // A stream connection has no record boundaries of its own, so the null
    // byte is what marks the end of one.
    size_t n = s.stream ? len + 1 : len;
    long ret = LIBC_NAMESPACE::syscall_impl<long>(SYS_sendto, s.fd, record, n,
                                                  MSG_NOSIGNAL, nullptr, 0);
    if (ret >= 0) {
      sent = true;
      break;
    }
    // The daemon may have been restarted since the connection was made, so
    // drop it and try once more on a fresh one.
    syslog_internal::disconnect_unlocked();
  }

  // Falling back to the console is only done when it was asked for, since it
  // writes where anyone logged in will see it.
  if (!sent && (option & LOG_CONS)) {
    int fd = LIBC_NAMESPACE::syscall_impl<int>(
        SYS_openat, AT_FDCWD, "/dev/console", O_WRONLY | O_NOCTTY, 0);
    if (fd >= 0) {
      LIBC_NAMESPACE::syscall_impl<long>(SYS_write, fd, record + header_len,
                                         len - header_len);
      LIBC_NAMESPACE::syscall_impl<long>(SYS_write, fd, "\r\n", 2);
      LIBC_NAMESPACE::syscall_impl<int>(SYS_close, fd);
    }
  }
}

} // namespace

LLVM_LIBC_FUNCTION(void, vsyslog,
                   (int priority, const char *format, va_list ap)) {
  const int level = LOG_PRI(priority);

  cpp::lock_guard guard(syslog_internal::log_mutex);
  syslog_internal::State &s = syslog_internal::log_state;

  if ((LOG_MASK(level) & s.mask) == 0)
    return;

  // A priority which names no facility of its own takes the one openlog set.
  int facility = priority & LOG_FACMASK;
  if (facility == 0)
    facility = s.facility;

  char record[RECORD_MAX];
  Buffer out(record, RECORD_MAX - 1);

  out.put('<');
  out.put_int(facility | level);
  out.put('>');
  put_timestamp(out);

  // Where the message starts, which is what goes to the console and to
  // stderr; those want the text without the wire header.
  const size_t header_len = out.size();

  // A program which passed no ident to openlog is tagged with its own name,
  // which is more use to whoever reads the log than no tag at all.
  const char *tag = s.ident != nullptr
                        ? s.ident
                        : LIBC_NAMESPACE::program_invocation_short_name;
  if (tag != nullptr)
    out.put(tag);
  if (s.option & LOG_PID) {
    out.put('[');
    out.put_int(LIBC_NAMESPACE::syscall_impl<int>(SYS_getpid));
    out.put(']');
  }
  if (tag != nullptr || (s.option & LOG_PID)) {
    out.put(':');
    out.put(' ');
  }

  char scratch[FORMAT_MAX];
  const char *fmt = expand_errno(format, scratch, FORMAT_MAX);

  internal::ArgList args(ap);
  printf_core::DropOverflowBuffer wb(out.end(), out.room());
  printf_core::Writer writer(wb);
  auto written = printf_core::printf_main(&writer, fmt, args);

  size_t len = out.size();
  if (written.has_value()) {
    size_t n = written.value();
    len += n < out.room() ? n : out.room();
  }
  record[len] = '\0';

  send_record(record, len, header_len, s.option);
}

} // namespace LIBC_NAMESPACE_DECL
