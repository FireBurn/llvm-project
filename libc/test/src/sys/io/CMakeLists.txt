add_custom_target(libc_sys_io_unittests)

add_libc_test(
  io_test
  SUITE
    libc_sys_io_unittests
  SRCS
    io_test.cpp
  DEPENDS
    libc.hdr.errno_macros
    libc.src.__support.libc_errno
    libc.src.sys.io.ioperm
    libc.src.sys.io.iopl
    libc.src.unistd.geteuid
    libc.test.UnitTest.ErrnoCheckingTest
    libc.test.UnitTest.ErrnoSetterMatcher
)
