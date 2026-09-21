if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${LIBC_TARGET_OS})
  add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/${LIBC_TARGET_OS})
endif()

add_entrypoint_object(
  sysinfo
  ALIAS
  DEPENDS
    .${LIBC_TARGET_OS}.sysinfo
)

add_entrypoint_object(
  get_nprocs
  ALIAS
  DEPENDS
    .${LIBC_TARGET_OS}.get_nprocs
)

add_entrypoint_object(
  get_nprocs_conf
  ALIAS
  DEPENDS
    .${LIBC_TARGET_OS}.get_nprocs_conf
)
