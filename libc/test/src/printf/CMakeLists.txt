add_libc_testsuite(libc-printf-tests)

add_libc_test(
  parse_printf_format_test
  SUITE
    libc-printf-tests
  SRCS
    parse_printf_format_test.cpp
  DEPENDS
    libc.hdr.printf_macros
    libc.src.printf.parse_printf_format
)
