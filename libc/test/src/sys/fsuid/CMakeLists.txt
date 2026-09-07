add_libc_testsuite(libc-sys-fsuid-tests)

add_libc_test(
  setfsuid_test
  SUITE
    libc-sys-fsuid-tests
  SRCS
    setfsuid_test.cpp
  DEPENDS
    libc.src.sys.fsuid.setfsgid
    libc.src.sys.fsuid.setfsuid
    libc.src.unistd.getgid
    libc.src.unistd.getuid
)
