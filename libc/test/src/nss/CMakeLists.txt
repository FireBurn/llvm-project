add_libc_testsuite(libc-nss-tests)

if(NOT TARGET libc.src.pwd.pwd_utils OR NOT TARGET libc.src.grp.grp_utils)
  return()
endif()

add_libc_test(
  nss_databases_test
  SUITE
    libc-nss-tests
  HDRS
    ../pwd/pwd_test_utils.h
  SRCS
    nss_databases_test.cpp
  DEPENDS
    libc.hdr.errno_macros
    libc.hdr.types.struct_group
    libc.hdr.types.struct_passwd
    libc.src.__support.CPP.string_view
    libc.src.__support.nss.config
    libc.src.__support.nss.module
    libc.src.grp.getgrgid
    libc.src.grp.getgrnam
    libc.src.grp.getgrouplist
    libc.src.pwd.endpwent
    libc.src.pwd.getpwent
    libc.src.pwd.getpwnam
    libc.src.pwd.getpwnam_r
    libc.src.pwd.getpwuid
    libc.src.pwd.setpwent
    libc.src.stdio.remove
    libc.src.string.memcpy
    libc.src.string.string_utils
)
