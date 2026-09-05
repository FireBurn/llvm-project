add_custom_target(libc_sys_timex_unittests)

add_libc_test(
  timex_test
  SUITE
    libc_sys_timex_unittests
  SRCS
    timex_test.cpp
  DEPENDS
    libc.hdr.errno_macros
    libc.hdr.time_macros
    libc.src.__support.libc_errno
    libc.include.sys_timex
    libc.src.sys.timex.adjtimex
    libc.src.sys.timex.clock_adjtime
    libc.src.sys.timex.ntp_adjtime
    libc.test.UnitTest.ErrnoCheckingTest
)
