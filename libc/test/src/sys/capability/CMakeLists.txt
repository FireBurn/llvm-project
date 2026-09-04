add_libc_testsuite(libc-sys-capability-tests)

add_libc_test(
  capability_test
  SUITE
    libc-sys-capability-tests
  SRCS
    capability_test.cpp
  DEPENDS
    libc.hdr.errno_macros
    libc.src.sys.capability.capget
    libc.src.sys.capability.capset
    libc.test.UnitTest.ErrnoCheckingTest
    libc.test.UnitTest.ErrnoSetterMatcher
)
