if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${LIBC_TARGET_OS})
  add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/${LIBC_TARGET_OS})
endif()

add_entrypoint_object(
  arch_prctl
  ALIAS
  DEPENDS
    .${LIBC_TARGET_OS}.arch_prctl
)

add_entrypoint_object(
  prctl
  ALIAS
  DEPENDS
    .${LIBC_TARGET_OS}.prctl
)
