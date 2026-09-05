add_custom_target(libc_ucontext_unittests)

if(TARGET libc.src.ucontext.getcontext)
  add_libc_test(
    ucontext_test
    SUITE
      libc_ucontext_unittests
    SRCS
      ucontext_test.cpp
    DEPENDS
      libc.src.ucontext.getcontext
      libc.src.ucontext.setcontext
      libc.src.signal.sigemptyset
      libc.src.signal.sigaddset
      libc.src.signal.sigprocmask
  )
endif()

if(TARGET libc.src.ucontext.swapcontext)
  add_libc_test(
    swapcontext_test
    SUITE
      libc_ucontext_unittests
    SRCS
      swapcontext_test.cpp
    DEPENDS
      libc.include.ucontext
      libc.src.ucontext.getcontext
      libc.src.ucontext.makecontext
      libc.src.ucontext.swapcontext
  )
endif()
