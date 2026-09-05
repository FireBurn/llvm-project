add_libc_testsuite(libc-sys-times-tests)

add_libc_test(
  times_test
  SUITE
    libc-sys-times-tests
  SRCS
    times_test.cpp
  DEPENDS
    libc.hdr.types.clock_t
    libc.hdr.types.struct_tms
    libc.src.sys.times.times
    libc.test.UnitTest.ErrnoCheckingTest
)
