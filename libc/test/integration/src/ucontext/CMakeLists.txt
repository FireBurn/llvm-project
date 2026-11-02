add_libc_testsuite(libc-ucontext-integration-tests PARENT libc-integration-tests)

# The test checks registers by name in x86_64 assembly.
if(LIBC_TARGET_ARCHITECTURE_IS_X86_64)
  add_integration_test(
    ucontext_test
    SUITE
      libc-ucontext-integration-tests
    SRCS
      ucontext_test.cpp
    DEPENDS
      libc.src.ucontext.getcontext
      libc.src.ucontext.setcontext
  )
endif()
