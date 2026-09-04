add_custom_target(libc_sys_swap_unittests)

add_libc_test(
  swap_test
  SUITE
    libc_sys_swap_unittests
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
