add_libc_testsuite(libc-sys-module-tests)

add_libc_test(
  module_test
  SUITE
    libc-sys-module-tests
  SRCS
    module_test.cpp
  DEPENDS
    libc.hdr.errno_macros
    libc.src.__support.libc_errno
    libc.src.sys.module.delete_module
    libc.src.sys.module.init_module
    libc.src.unistd.geteuid
    libc.test.UnitTest.ErrnoCheckingTest
)
