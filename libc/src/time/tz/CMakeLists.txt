add_object_library(
  posix_tz
  SRCS
    posix_tz.cpp
  HDRS
    posix_tz.h
  DEPENDS
    libc.hdr.stdint_proxy
    libc.hdr.types.size_t
    libc.hdr.types.time_t
    libc.src.__support.ctype_utils
    libc.src.__support.macros.config
    libc.src.time.time_constants
    libc.src.time.time_utils
)

add_object_library(
  tzfile
  SRCS
    tzfile.cpp
  HDRS
    tzfile.h
  DEPENDS
    libc.src.__support.macros.attributes
    .posix_tz
    libc.hdr.stdint_proxy
    libc.hdr.types.size_t
    libc.hdr.types.time_t
    libc.src.__support.macros.config
    libc.src.string.memory_utils.inline_memcpy
)

add_header_library(
  zone_source_header
  HDRS
    zone_source.h
  DEPENDS
    libc.hdr.types.size_t
    libc.src.__support.macros.config
)

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${LIBC_TARGET_OS})
  add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/${LIBC_TARGET_OS})
  set(zone_source .${LIBC_TARGET_OS}.zone_source)
else()
  add_subdirectory(generic)
  set(zone_source .generic.zone_source)
endif()

add_object_library(
  timezone
  SRCS
    timezone.cpp
  HDRS
    timezone.h
  DEPENDS
    ${zone_source}
    .posix_tz
    .tzfile
    libc.hdr.types.struct_tm
    libc.hdr.types.time_t
    libc.src.__support.CPP.mutex
    libc.src.__support.error_or
    libc.src.__support.macros.config
    libc.src.__support.threads.mutex
    libc.src.string.memory_utils.inline_memcpy
    libc.src.time.time_constants
    libc.src.time.time_utils
    libc.src.time.tz_variables
)
