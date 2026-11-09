add_libc_testsuite(libc-sys-prctl-tests)

if(LIBC_TARGET_ARCHITECTURE_IS_X86_64)
  add_libc_test(
    arch_prctl_test
    SUITE
      libc-sys-prctl-tests
    SRCS
      arch_prctl_test.cpp
    DEPENDS
      libc.src.sys.prctl.arch_prctl
      libc.test.UnitTest.ErrnoCheckingTest
      libc.test.UnitTest.ErrnoSetterMatcher
  )
endif()

# Temporarily disable this test while setting up arm and riscv buildbots
# using qemu, since PR_GET_THP_DISABLE is not supported on qemu.
if(NOT (LIBC_TARGET_ARCHITECTURE_IS_ARM OR
    LIBC_TARGET_ARCHITECTURE_IS_RISCV32 OR
    LIBC_TARGET_ARCHITECTURE_IS_RISCV64))
  add_libc_test(
    prctl_test
    SUITE
      libc-sys-prctl-tests
    SRCS
      prctl_test.cpp
    DEPENDS
      libc.include.sys_prctl
      libc.src.sys.prctl.prctl
      libc.test.UnitTest.ErrnoCheckingTest
      libc.test.UnitTest.ErrnoSetterMatcher
  )
endif()
