add_entrypoint_object(
  adjtimex
  SRCS
    adjtimex.cpp
  HDRS
    adjtimex.h
  DEPENDS
    libc.hdr.types.struct_timex
    libc.src.__support.OSUtil.linux.syscall_wrappers.adjtimex
    libc.src.errno.errno
)

add_entrypoint_object(
  ntp_adjtime
  SRCS
    ntp_adjtime.cpp
  HDRS
    ntp_adjtime.h
  DEPENDS
    libc.hdr.types.struct_timex
    libc.src.__support.OSUtil.linux.syscall_wrappers.adjtimex
    libc.src.errno.errno
)

add_entrypoint_object(
  clock_adjtime
  SRCS
    clock_adjtime.cpp
  HDRS
    clock_adjtime.h
  DEPENDS
    libc.hdr.types.clockid_t
    libc.hdr.types.struct_timex
    libc.src.__support.OSUtil.linux.syscall_wrappers.clock_adjtime
    libc.src.errno.errno
)

