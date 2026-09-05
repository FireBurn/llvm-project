add_custom_target(libc_sys_times_unittests)

add_libc_test(
  times_test
  SUITE
    libc_sys_times_unittests
  SRCS
    times_test.cpp
  DEPENDS
    libc.hdr.types.clock_t
    libc.hdr.types.struct_tms
    libc.src.sys.times.times
    libc.test.UnitTest.ErrnoCheckingTest
)
