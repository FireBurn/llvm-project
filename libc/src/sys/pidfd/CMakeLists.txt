if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${LIBC_TARGET_OS})
  add_subdirectory(${LIBC_TARGET_OS})
endif()

add_entrypoint_object(
  pidfd_open
  ALIAS
  DEPENDS
    .${LIBC_TARGET_OS}.pidfd_open
)

add_entrypoint_object(
  pidfd_getfd
  ALIAS
  DEPENDS
    .${LIBC_TARGET_OS}.pidfd_getfd
)

add_entrypoint_object(
  pidfd_send_signal
  ALIAS
  DEPENDS
    .${LIBC_TARGET_OS}.pidfd_send_signal
)
