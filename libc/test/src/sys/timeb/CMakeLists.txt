add_libc_testsuite(libc-sys-timeb-tests)

add_libc_test(
  ftime_test
  SUITE
    libc-sys-timeb-tests
  SRCS
    ftime_test.cpp
  DEPENDS
    libc.hdr.types.struct_timeb
    libc.src.sys.timeb.ftime
    libc.src.time.time
    libc.test.UnitTest.ErrnoCheckingTest
)
