add_custom_target(libc_sys_quota_unittests)

add_libc_test(
  quotactl_test
  SUITE
    libc_sys_quota_unittests
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
