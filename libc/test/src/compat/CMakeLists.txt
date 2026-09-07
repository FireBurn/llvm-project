add_custom_target(libc_compat_unittests)

add_libc_test(
  compat_test
  SUITE
    libc_compat_unittests
  SRCS
    compat_test.cpp
  DEPENDS
    libc.hdr.errno_macros
    libc.src.__support.libc_errno
    libc.src.compat.__errno_location
    libc.src.compat.__xpg_strerror_r
    libc.src.compat.gnu_get_libc_release
    libc.src.compat.gnu_get_libc_version
    libc.src.string.strlen
)
