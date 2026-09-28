add_libc_testsuite(libc-link-tests)

add_libc_test(
  dl_iterate_phdr_test
  SUITE
    libc-link-tests
  SRCS
    dl_iterate_phdr_test.cpp
  DEPENDS
    libc.src.link.dl_iterate_phdr
)

if(LIBC_ENABLE_SHARED)
  add_libc_test(
    link_maps_test
    SUITE
      libc-link-tests
    SRCS
      link_maps_test.cpp
    COMPILE_OPTIONS
      -DLIBC_TEST_SHARED_OBJECT="$<TARGET_FILE:libc-shared>"
      -DLIBM_TEST_SHARED_OBJECT="$<TARGET_FILE:libm-shared>"
    DEPENDS
      libc.src.__support.elf.load_module
      libc.src.link.link_maps
  )
endif()
