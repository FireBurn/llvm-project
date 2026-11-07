add_libc_testsuite(libc-assert-tests)

add_libc_test(
  assert_test
  SUITE
    libc-assert-tests
  SRCS
    assert_test.cpp
  DEPENDS
    libc.src.assert.__assert_fail
    libc.src.unistd.close
    # These are necessary for now because dependencies are not properly added.
    libc.src.signal.raise
    libc.src.stdlib._Exit
    libc.src.stdlib.abort
)

add_libc_test(
  assert_perror_test
  SUITE
    libc-assert-tests
  SRCS
    assert_perror_test.cpp
  DEPENDS
    libc.hdr.errno_macros
    libc.hdr.signal_macros
    libc.src.assert.__assert_perror_fail
    libc.src.unistd.close
    libc.src.signal.raise
    libc.src.stdlib._Exit
    libc.src.stdlib.abort
)

add_libc_test(
  assert2_test
  SUITE
    libc-assert-tests
  SRCS
    assert2_test.cpp
  DEPENDS
    libc.hdr.signal_macros
    libc.src.assert.__assert
    libc.src.assert.__assert2
    libc.src.unistd.close
    libc.src.signal.raise
    libc.src.stdlib._Exit
    libc.src.stdlib.abort
)

