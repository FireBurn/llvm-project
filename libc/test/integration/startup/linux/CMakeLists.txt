if(NOT (EXISTS ${LIBC_SOURCE_DIR}/startup/linux/${LIBC_TARGET_ARCHITECTURE}))
  message("Skipping startup integration tests for target architecture ${LIBC_TARGET_ARCHITECTURE}.")
  return()
endif()

add_libc_testsuite(libc-startup-tests PARENT libc-integration-tests)

add_integration_test(
  progname_test
  SUITE libc-startup-tests
  SRCS
    progname_test.cpp
  DEPENDS
    libc.src.compat.progname
    libc.src.errno.program_invocation_name
    libc.src.errno.program_invocation_short_name
)

add_integration_test(
  debuggerd_hook_test
  SUITE libc-startup-tests
  SRCS
    debuggerd_hook_test.cpp
  DEPENDS
    libc.src.android.android_crash_detail_register
    libc.src.android.android_set_abort_message
    libc.src.android.debuggerd_hook
)

add_integration_test(
  crt_objects_elf_test
  SUITE libc-startup-tests
  SRCS
    crt_objects_elf_test.cpp
  DEPENDS
    libc.hdr.fcntl_macros
    libc.src.fcntl.open
    libc.src.unistd.close
    libc.src.unistd.read
    libc.startup.linux.crti
    libc.startup.linux.crtn
  ARGS
    $<TARGET_OBJECTS:libc.startup.linux.crti>
    $<TARGET_OBJECTS:libc.startup.linux.crtn>
)

add_integration_test(
  startup_args_test
  SUITE libc-startup-tests
  SRCS
    args_test.cpp
  ARGS
    1 2 3
  ENV
    FRANCE=Paris
    GERMANY=Berlin
)

add_integration_test(
  invocation_name_test
  SUITE libc-startup-tests
  SRCS
    invocation_name_test.cpp
  DEPENDS
    libc.src.__support.CPP.string_view
    libc.src.errno.program_invocation_name
    libc.src.errno.program_invocation_short_name
    libc.src.unistd.execve
)

add_integration_test(
  startup_no_envp_test
  SUITE libc-startup-tests
  SRCS
    main_without_envp.cpp
)

add_integration_test(
  startup_no_args_test
  SUITE libc-startup-tests
  SRCS
    main_without_args.cpp
)

add_integration_test(
  startup_tls_test
  SUITE libc-startup-tests
  SRCS
    tls_test.cpp
  DEPENDS
    libc.hdr.sys_mman_macros
    libc.include.sys_mman
    libc.src.sys.mman.mmap
)

add_integration_test(
  init_fini_array_test
  SUITE libc-startup-tests
  SRCS
    init_fini_array_test.cpp
)

add_integration_test(
  startup_irelative_test
  SUITE libc-startup-tests
  SRCS
    irelative_test.cpp
)

add_integration_test(
  startup_irelative_no_ifunc_test
  SUITE libc-startup-tests
  SRCS
    irelative_no_ifunc_test.cpp
)

add_integration_test(
  startup_r_debug_test
  SUITE libc-startup-tests
  SRCS
    r_debug_test.cpp
  DEPENDS
    libc.include.link
    libc.src.link._r_debug
)

if(LIBC_TARGET_ARCHITECTURE_IS_AARCH64)
  add_integration_test(
    startup_mte_test
    SUITE libc-startup-tests
    SRCS
      mte_test.cpp
    DEPENDS
      libc.hdr.sys_auxv_macros
      libc.src.sys.auxv.getauxval
  )
endif()
