add_libc_testsuite(libc-sys-io-tests)

add_libc_test(
  io_test
  SUITE
    libc-sys-io-tests
  SRCS
    io_test.cpp
  DEPENDS
    libc.hdr.errno_macros
    libc.src.__support.libc_errno
    libc.src.sys.io.ioperm
    libc.src.sys.io.iopl
    libc.src.unistd.geteuid
    libc.test.UnitTest.ErrnoCheckingTest
)
