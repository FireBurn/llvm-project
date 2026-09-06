if(NOT LIBC_TARGET_OS_IS_LINUX)
  return()
endif()

add_subdirectory(${LIBC_TARGET_OS})

add_entrypoint_object(
  getifaddrs
  ALIAS
  DEPENDS
    .${LIBC_TARGET_OS}.getifaddrs
)

add_entrypoint_object(
  freeifaddrs
  ALIAS
  DEPENDS
    .${LIBC_TARGET_OS}.freeifaddrs
)
