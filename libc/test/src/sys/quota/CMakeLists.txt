add_libc_testsuite(libc-sys-quota-tests)

add_libc_test(
  quotactl_test
  SUITE
    libc-sys-quota-tests
  SRCS
    quotactl_test.cpp
  DEPENDS
    libc.hdr.errno_macros
    libc.include.sys_quota
    libc.src.__support.libc_errno
    libc.src.sys.quota.quotactl
    libc.src.sys.quota.quotactl_fd
    libc.test.UnitTest.ErrnoCheckingTest
)
