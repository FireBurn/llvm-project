add_custom_target(libc_sys_mount_unittests)

add_libc_test(
  mount_test
  SUITE
    libc_sys_mount_unittests
  SRCS
    mount_test.cpp
  DEPENDS
    libc.hdr.errno_macros
    libc.hdr.sys_mount_macros
    libc.src.errno.errno
    libc.src.sys.mount.mount
    libc.src.sys.mount.umount
    libc.src.sys.mount.umount2
    libc.test.UnitTest.ErrnoCheckingTest
    libc.test.UnitTest.ErrnoSetterMatcher
)
