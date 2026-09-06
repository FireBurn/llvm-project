add_custom_target(libc_error_unittests)

if(NOT TARGET libc.src.__support.OSUtil.osutil)
  return()
endif()

add_libc_test(
  error_test
  SUITE
    libc_error_unittests
  SRCS
    error_test.cpp
  DEPENDS
    libc.src.error.error
    libc.src.error.error_message_count
    libc.src.error.error_print_progname
)

add_libc_test(
  error_at_line_test
  SUITE
    libc_error_unittests
  SRCS
    error_at_line_test.cpp
  DEPENDS
    libc.src.error.error_at_line
    libc.src.error.error_message_count
    libc.src.error.error_one_per_line
)
