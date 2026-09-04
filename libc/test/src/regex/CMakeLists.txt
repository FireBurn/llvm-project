add_custom_target(libc_regex_unittests)

add_libc_test(
  regex_test
  SUITE
    libc_regex_unittests
  SRCS
    regex_test.cpp
  DEPENDS
    libc.hdr.regex_macros
    libc.hdr.types.regex_t
    libc.hdr.types.regmatch_t
    libc.src.regex.regcomp
    libc.src.regex.regexec
    libc.src.regex.regfree
)
