if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${LIBC_TARGET_OS})
  message(STATUS "Skipping sys/times.h for ${LIBC_TARGET_OS}")
  return()
endif()

add_subdirectory(${LIBC_TARGET_OS})

add_entrypoint_object(
  times
  ALIAS
  DEPENDS
    .${LIBC_TARGET_OS}.times
)
