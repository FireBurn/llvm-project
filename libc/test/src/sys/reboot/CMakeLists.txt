add_custom_target(libc_sys_reboot_unittests)

add_libc_test(
  reboot_test
  SUITE
    libc_sys_reboot_unittests
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
