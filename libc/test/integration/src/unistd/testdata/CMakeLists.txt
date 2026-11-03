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

# A script without a shebang to test fallback execution via /bin/sh on ENOEXEC.
set(shebangless_file
  ${CMAKE_CURRENT_BINARY_DIR}/shebangless.sh)
file(WRITE ${shebangless_file} "if [ -n \"\${1:-}\" ]; then exit \"\$1\"; fi\nexit 42\n")
file(CHMOD ${shebangless_file}
  PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
