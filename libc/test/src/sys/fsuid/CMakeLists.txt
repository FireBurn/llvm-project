add_custom_target(libc_sys_fsuid_unittests)

add_libc_test(
  setfsuid_test
  SUITE
    libc_sys_fsuid_unittests
  SRCS
    setfsuid_test.cpp
  DEPENDS
    libc.src.sys.fsuid.setfsgid
    libc.src.sys.fsuid.setfsuid
    libc.src.unistd.getgid
    libc.src.unistd.getuid
)
