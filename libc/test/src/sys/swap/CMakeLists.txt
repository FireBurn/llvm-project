add_libc_testsuite(libc-sys-swap-tests)

add_libc_test(
  swap_test
  SUITE
    libc-sys-swap-tests
  SRCS
    swap_test.cpp
  DEPENDS
    libc.hdr.errno_macros
    libc.hdr.sys_swap_macros
    libc.src.errno.errno
    libc.src.sys.swap.swapoff
    libc.src.sys.swap.swapon
    libc.test.UnitTest.ErrnoCheckingTest
    libc.test.UnitTest.ErrnoSetterMatcher
)
