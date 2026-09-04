add_custom_target(libc_sys_capability_unittests)

add_libc_test(
  capability_test
  SUITE
    libc_sys_capability_unittests
  SRCS
    capability_test.cpp
  DEPENDS
    libc.hdr.errno_macros
    libc.src.sys.capability.capget
    libc.src.sys.capability.capset
    libc.test.UnitTest.ErrnoCheckingTest
    libc.test.UnitTest.ErrnoSetterMatcher
)
