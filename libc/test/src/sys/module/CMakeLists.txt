add_custom_target(libc_sys_module_unittests)

add_libc_test(
  module_test
  SUITE
    libc_sys_module_unittests
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
