//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Linux implementation of internal state and formatting for syslog.
///
//===----------------------------------------------------------------------===//

#include "hdr/fcntl_macros.h"
#include "hdr/sys_socket_macros.h"
#include "hdr/syslog_macros.h"
#include "hdr/time_macros.h"
#include "hdr/types/pid_t.h"
#include "hdr/types/size_t.h"
#include "hdr/types/socklen_t.h"
#include "hdr/types/struct_sockaddr.h"
#include "hdr/types/struct_sockaddr_un.h"
#include "hdr/types/struct_timespec.h"
#include "hdr/types/struct_tm.h"
#include "src/__support/CPP/string_view.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/close.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/connect.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/getpid.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/openat.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/sendto.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/socket.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/write.h"
#include "src/__support/arg_list.h"
#include "src/__support/integer_to_string.h"
#include "src/__support/macros/attributes.h"
#include "src/__support/macros/config.h"
#include "src/__support/printf_core/core_structs.h"
#include "src/__support/printf_core/printf_main.h"
#include "src/__support/printf_core/writer.h"
#include "src/__support/threads/mutex.h"
#include "src/__support/time/clock_gettime.h"
#include "src/errno/program_invocation_short_name.h"
#include "src/string/memory_utils/inline_memcpy.h"
#include "src/syslog/linux/syslog_internal.h"
#include "src/time/strftime_core/strftime_main.h"
#include "src/time/time_utils.h"


namespace LIBC_NAMESPACE_DECL {
namespace syslog_internal {

namespace {

constexpr size_t BUFFER_SIZE = 256;
constexpr size_t RECORD_SIZE = 2048;
// The length of "Mmm dd HH:MM:SS ".
constexpr size_t TIMESTAMP_LENGTH = 16;

Mutex log_mutex(/*is_priority_inherit=*/false, /*is_recursive=*/false,
                /*is_robust=*/false, /*is_pshared=*/false);
LIBC_CONSTINIT detail::LogState log_state;

// Guarded by log_mutex.
const char *log_path = _PATH_LOG;
int log_fd = -1;
bool log_is_stream = false;

// A record is sent whole, as one datagram, so it is collected before sending.
struct Record {
  char data[RECORD_SIZE];
  size_t length = 0;

  LIBC_INLINE static int append(cpp::string_view str, void *target) {
    auto *record = static_cast<Record *>(target);
    // The last byte is kept for the terminator a stream socket needs.
    const size_t room = sizeof(record->data) - 1 - record->length;
    const size_t count = str.size() < room ? str.size() : room;
    inline_memcpy(record->data + record->length, str.data(), count);
    record->length += count;
    return printf_core::WRITE_OK;
  }
};

int open_socket(int type) {
  ErrorOr<int> fd = linux_syscalls::socket(AF_UNIX, type | SOCK_CLOEXEC, 0);
  if (!fd)
    return -1;
  struct sockaddr_un addr = {};
  addr.sun_family = AF_UNIX;
  for (size_t i = 0; log_path[i] != '\0' && i + 1 < sizeof(addr.sun_path); ++i)
    addr.sun_path[i] = log_path[i];
  if (!linux_syscalls::connect(*fd, reinterpret_cast<struct sockaddr *>(&addr),
                               static_cast<socklen_t>(sizeof(addr)))) {
    linux_syscalls::close(*fd);
    return -1;
  }
  return *fd;
}

bool connect_unlocked() {
  if (log_fd >= 0)
    return true;
  // The daemon usually listens on a datagram socket, but may use a stream one.
  log_is_stream = false;
  log_fd = open_socket(SOCK_DGRAM);
  if (log_fd < 0) {
    log_fd = open_socket(SOCK_STREAM);
    log_is_stream = log_fd >= 0;
  }
  return log_fd >= 0;
}

void disconnect_unlocked() {
  if (log_fd >= 0)
    linux_syscalls::close(log_fd);
  log_fd = -1;
}

void send_record(const Record &record, int options) {
  // What follows the priority and the timestamp is what a person reads.
  size_t text = 0;
  while (text < record.length && record.data[text] != '>')
    ++text;
  text += 1 + TIMESTAMP_LENGTH;
  if (text > record.length)
    text = record.length;

  if (options & LOG_PERROR) {
    linux_syscalls::write(2, record.data + text, record.length - text);
    linux_syscalls::write(2, "\n", 1);
  }

  bool sent = false;
  // A second attempt reconnects to a daemon that has been restarted.
  for (int attempt = 0; attempt < 2 && !sent; ++attempt) {
    if (!connect_unlocked())
      break;
    const size_t length = record.length + (log_is_stream ? 1 : 0);
    if (linux_syscalls::sendto(log_fd, record.data, length, MSG_NOSIGNAL,
                               nullptr, 0))
      sent = true;
    else
      disconnect_unlocked();
  }

  if (!sent && (options & LOG_CONS)) {
    ErrorOr<int> console = linux_syscalls::openat(AT_FDCWD, "/dev/console",
                                                  O_WRONLY | O_NOCTTY, 0);
    if (console) {
      linux_syscalls::write(*console, record.data + text, record.length - text);
      linux_syscalls::write(*console, "\r\n", 2);
      linux_syscalls::close(*console);
    }
  }
}

} // namespace

namespace detail {

bool write_log_message(
    printf_core::Writer<printf_core::OverflowMode::CALLBACK> &writer,
    const LogState &state, int priority, const char *__restrict format,
    internal::ArgList &args, const tm &timestamp, pid_t pid,
    const char *default_ident) {
  if (!state.allows_priority(priority))
    return false;
  if ((priority & FACILITY_MASK) == 0)
    priority |= state.facility;

  const char *ident = state.ident;
  if (ident == nullptr)
    ident = default_ident;
  if (ident == nullptr)
    ident = "";

  writer.write('<');
  const IntegerToString<int> priority_str(priority);
  writer.write(priority_str.view());
  writer.write('>');

  // RFC 3164 timestamp format: abbreviated month name, space-padded day of
  // month, and 24-hour HH:MM:SS time
  // (e.g. "Nov 14 01:02:03 " or "Jan  5 23:59:58 ").
  if (!strftime_core::strftime_main(&writer, "%b %e %T ", &timestamp)
           .has_value())
    return false;

  writer.write(ident);
  if (state.options & LOG_PID) {
    writer.write('[');
    const IntegerToString<pid_t> pid_str(pid);
    writer.write(pid_str.view());
    writer.write(']');
  }
  writer.write(cpp::string_view(": "));

  if (!printf_core::printf_main(&writer, format, args).has_value())
    return false;

  const auto &wb = writer.get_write_buffer();
  if (wb.buff_cur == 0 || wb.buff[wb.buff_cur - 1] != '\n')
    writer.write('\n');

  return true;
}

} // namespace detail

void open_log(const char *ident, int options, int facility) {
  log_mutex.lock();
  log_state.configure(ident, options, facility);
  if (options & LOG_NDELAY)
    connect_unlocked();
  log_mutex.unlock();
}

void close_log() {
  log_mutex.lock();
  disconnect_unlocked();
  log_state.reset();
  log_mutex.unlock();
}

void set_log_path(const char *path) {
  log_mutex.lock();
  disconnect_unlocked();
  log_path = path == nullptr ? _PATH_LOG : path;
  log_mutex.unlock();
}

int set_log_mask(int mask) {
  log_mutex.lock();
  int old_mask = log_state.update_mask(mask);
  log_mutex.unlock();
  return old_mask;
}

void log_message(int priority, const char *__restrict format,
                 internal::ArgList &args) {
  log_mutex.lock();
  if (log_state.allows_priority(priority)) {
    timespec now = {};
    internal::clock_gettime(CLOCK_REALTIME, &now);
    tm local_time = {};
    if (time_utils::localtime_internal(&now.tv_sec, &local_time).has_value()) {
      pid_t pid = (log_state.options & LOG_PID) ? linux_syscalls::getpid() : 0;
      Record record;
      char buffer[BUFFER_SIZE];
      printf_core::Writer writer = printf_core::make_writer(
          buffer, sizeof(buffer),
          &printf_core::overflow_write_flush_to_sink<char, Record::append>,
          &record);
      if (detail::write_log_message(writer, log_state, priority, format, args,
                                    local_time, pid,
                                    program_invocation_short_name)) {
        writer.get_write_buffer().flush_to_sink<Record::append>(&record);
        if (record.length > 0 && record.data[record.length - 1] == '\n')
          --record.length;
        record.data[record.length] = '\0';
        send_record(record, log_state.options);
      }
    }
  }
  log_mutex.unlock();
}

} // namespace syslog_internal
} // namespace LIBC_NAMESPACE_DECL
