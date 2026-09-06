if(NOT TARGET libc.src.__support.OSUtil.osutil)
  return()
endif()

add_entrypoint_object(
  error_message_count
  SRCS
    error_message_count.cpp
  HDRS
    error_message_count.h
  DEPENDS
    libc.src.__support.common
    libc.src.__support.macros.config
)

add_entrypoint_object(
  error_one_per_line
  SRCS
    error_one_per_line.cpp
  HDRS
    error_one_per_line.h
  DEPENDS
    libc.src.__support.common
    libc.src.__support.macros.config
)

add_entrypoint_object(
  error_print_progname
  SRCS
    error_print_progname.cpp
  HDRS
    error_print_progname.h
  DEPENDS
    libc.src.__support.common
    libc.src.__support.macros.config
)

set(report_extra_deps)
if(LLVM_LIBC_FULL_BUILD)
  list(APPEND report_extra_deps libc.src.errno.program_invocation_name)
endif()

add_object_library(
  report
  SRCS
    report.cpp
  HDRS
    report.h
  DEPENDS
    .error_message_count
    .error_print_progname
    libc.hdr.types.FILE
    libc.src.__support.CPP.string_view
    libc.src.__support.File.file
    libc.src.__support.OSUtil.osutil
    libc.src.__support.StringUtil.error_to_string
    libc.src.__support.arg_list
    libc.src.__support.macros.attributes
    libc.src.__support.macros.config
    libc.src.__support.printf_core.printf_main
    libc.src.__support.printf_core.writer
    ${report_extra_deps}
)

add_entrypoint_object(
  error
  SRCS
    error.cpp
  HDRS
    error.h
  DEPENDS
    .report
    libc.src.__support.OSUtil.osutil
    libc.src.__support.arg_list
    libc.src.__support.common
    libc.src.__support.macros.config
)

add_entrypoint_object(
  error_at_line
  SRCS
    error_at_line.cpp
  HDRS
    error_at_line.h
  DEPENDS
    .error_one_per_line
    .report
    libc.src.__support.OSUtil.osutil
    libc.src.__support.arg_list
    libc.src.__support.common
    libc.src.__support.macros.config
    libc.src.string.strcmp
)
