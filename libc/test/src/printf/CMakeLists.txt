add_custom_target(libc_printf_unittests)

add_libc_test(
  parse_printf_format_test
  SUITE
    libc_printf_unittests
  SRCS
    parse_printf_format_test.cpp
  DEPENDS
    libc.hdr.printf_macros
    libc.src.printf.parse_printf_format
)
