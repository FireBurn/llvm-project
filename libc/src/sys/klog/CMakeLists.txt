if(NOT LIBC_TARGET_OS_IS_LINUX)
  return()
endif()

add_subdirectory(${LIBC_TARGET_OS})

add_entrypoint_object(
  klogctl
  ALIAS
  DEPENDS
    .${LIBC_TARGET_OS}.klogctl
)
