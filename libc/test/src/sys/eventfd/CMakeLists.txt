if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${LIBC_TARGET_OS})
  add_subdirectory(${LIBC_TARGET_OS})
endif()

add_libc_test(
  eventfd_test
  SUITE
    libc-sys-eventfd-tests
  SRCS
    eventfd_test.cpp
  DEPENDS
    libc.hdr.errno_macros
    libc.hdr.sys_eventfd_macros
    libc.hdr.types.eventfd_t
    libc.src.errno.errno
    libc.src.sys.eventfd.eventfd
    libc.src.sys.eventfd.eventfd_read
    libc.src.sys.eventfd.eventfd_write
    libc.src.unistd.close
    libc.test.UnitTest.ErrnoCheckingTest
    libc.test.UnitTest.ErrnoSetterMatcher
)
