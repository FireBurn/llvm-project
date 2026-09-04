add_libc_testsuite(libc-sys-mount-tests)

add_libc_test(
  mount_test
  SUITE
    libc-sys-mount-tests
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
