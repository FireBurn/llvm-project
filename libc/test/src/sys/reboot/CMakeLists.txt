add_libc_testsuite(libc-sys-reboot-tests)

add_libc_test(
  reboot_test
  SUITE
    libc-sys-reboot-tests
  SRCS
    reboot_test.cpp
  DEPENDS
    libc.hdr.errno_macros
    libc.include.sys_reboot
    libc.src.sys.reboot.reboot
    libc.src.unistd.geteuid
    libc.test.UnitTest.ErrnoCheckingTest
    libc.test.UnitTest.ErrnoSetterMatcher
)
