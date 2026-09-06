add_custom_target(libc_sys_klog_unittests)

add_libc_test(
  klogctl_test
  SUITE
    libc_sys_klog_unittests
  SRCS
    klogctl_test.cpp
  DEPENDS
    libc.hdr.errno_macros
    libc.src.__support.libc_errno
    libc.src.sys.klog.klogctl
    libc.test.UnitTest.ErrnoCheckingTest
)
