add_custom_target(libc_iconv_unittests)

add_libc_test(
  iconv_test
  SUITE
    libc_iconv_unittests
  SRCS
    iconv_test.cpp
  DEPENDS
    libc.hdr.errno_macros
    libc.hdr.types.iconv_t
    libc.hdr.types.ssize_t
    libc.src.errno.errno
    libc.src.iconv.iconv
    libc.src.iconv.iconv_close
    libc.src.iconv.iconv_open
    libc.test.UnitTest.ErrnoCheckingTest
)
