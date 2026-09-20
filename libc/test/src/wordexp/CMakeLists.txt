add_libc_testsuite(libc-wordexp-tests)

add_libc_test(
  wordexp_test
  SUITE
    libc-wordexp-tests
  SRCS
    wordexp_test.cpp
  DEPENDS
    libc.hdr.types.wordexp_t
    libc.hdr.wordexp_macros
    libc.src.stdlib.setenv
    libc.src.stdlib.unsetenv
    libc.src.wordexp.wordexp
    libc.src.wordexp.wordfree
    libc.test.UnitTest.ErrnoCheckingTest
)
