add_custom_target(libc_execinfo_unittests)

add_libc_test(
  backtrace_test
  SUITE
    libc_execinfo_unittests
  SRCS
    backtrace_test.cpp
  COMPILE_OPTIONS
    # The unwinder reads the tables the compiler emits for each frame, and
    # the tests are built without them by default.
    -funwind-tables
  DEPENDS
    libc.hdr.fcntl_macros
    libc.hdr.sys_stat_macros
    libc.src.execinfo.backtrace
    libc.src.execinfo.backtrace_symbols_fd
    libc.src.fcntl.open
    libc.src.sys.stat.fstat
    libc.src.unistd.close
    libc.src.unistd.unlink
)

# Naming the addresses returns one allocated block, so this is only built
# where the configuration has an allocator to free it with.
if(TARGET libc.src.stdlib.free)
  add_libc_test(
    backtrace_symbols_test
    SUITE
      libc_execinfo_unittests
    SRCS
      backtrace_symbols_test.cpp
    COMPILE_OPTIONS
      -funwind-tables
    DEPENDS
      libc.src.execinfo.backtrace
      libc.src.execinfo.backtrace_symbols
      libc.src.stdlib.free
      libc.src.string.strlen
  )
endif()
