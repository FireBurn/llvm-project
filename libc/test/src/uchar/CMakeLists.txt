add_custom_target(libc_uchar_unittests)

add_libc_test(
  mbrtoc32_test
  SUITE
    libc_uchar_unittests
  SRCS
    mbrtoc32_test.cpp
  DEPENDS
    libc.hdr.types.char32_t
    libc.hdr.types.mbstate_t
    libc.src.errno.errno
    libc.src.uchar.c32rtomb
    libc.src.uchar.mbrtoc32
    libc.test.UnitTest.ErrnoCheckingTest
)
