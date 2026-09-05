add_custom_target(libc_syslog_unittests)

add_libc_test(
  syslog_test
  SUITE
    libc_syslog_unittests
  SRCS
    syslog_test.cpp
  DEPENDS
    libc.hdr.syslog_macros
    libc.src.syslog.closelog
    libc.src.syslog.openlog
    libc.src.syslog.setlogmask
    libc.src.syslog.syslog
)
