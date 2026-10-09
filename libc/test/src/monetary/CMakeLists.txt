add_libc_testsuite(libc-monetary-tests)

add_libc_test(
  strfmon_test
  SUITE
    libc-monetary-tests
  SRCS
    strfmon_test.cpp
  DEPENDS
    libc.hdr.errno_macros
    libc.src.monetary.strfmon
    libc.src.monetary.strfmon_l
    libc.test.UnitTest.ErrnoCheckingTest
)
