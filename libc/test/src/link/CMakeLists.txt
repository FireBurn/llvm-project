add_custom_target(libc_link_unittests)

add_libc_test(
  dl_iterate_phdr_test
  SUITE
    libc_link_unittests
  SRCS
    dl_iterate_phdr_test.cpp
  DEPENDS
    libc.src.link.dl_iterate_phdr
)

if(LIBC_ENABLE_SHARED)
  add_libc_test(
    link_maps_test
    SUITE
      libc_link_unittests
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
