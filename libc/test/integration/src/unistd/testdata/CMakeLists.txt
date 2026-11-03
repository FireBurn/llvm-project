add_libc_test_helper(
  libc_execvp_test_normal_exit
  SRCS
  execvp_test_normal_exit.cpp
)

# A file with the same name as the helper binary but without any execute
# permission bits. Used to test that execvp continues searching PATH after
# EACCES and reports EACCES if no later PATH entry succeeds.
set(execvp_noexec_file
  ${CMAKE_CURRENT_BINARY_DIR}/noexec/libc_execvp_test_normal_exit)
file(WRITE ${execvp_noexec_file} "#!/bin/sh\nexit 1\n")
file(CHMOD ${execvp_noexec_file}
  PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)
