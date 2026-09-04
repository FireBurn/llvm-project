add_custom_target(libc_grp_unittests)

add_libc_test(
  setgroups_test
  SUITE
    libc_grp_unittests
  SRCS
    setgroups_test.cpp
  DEPENDS
    libc.hdr.errno_macros
    libc.hdr.limits_macros
    libc.hdr.types.gid_t
    libc.src.errno.errno
    libc.src.grp.setgroups
    libc.src.unistd.getgroups
    libc.test.UnitTest.ErrnoCheckingTest
    libc.test.UnitTest.ErrnoSetterMatcher
)
