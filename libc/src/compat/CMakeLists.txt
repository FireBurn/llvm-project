# Names the platform ABI settled on that are not what the standards call the
# functions behind them. Code already built, and code that states its own
# declarations, asks for these.

add_entrypoint_object(
  __gnu_strerror_r
  SRCS
    __gnu_strerror_r.cpp
  HDRS
    __gnu_strerror_r.h
  DEPENDS
    libc.hdr.types.size_t
    libc.src.__support.StringUtil.error_to_string
    libc.src.__support.common
    libc.src.__support.macros.config
)

add_entrypoint_object(
  gnu_get_libc_version
  SRCS
    gnu_get_libc_version.cpp
  HDRS
    gnu_get_libc_version.h
  DEPENDS
    libc.src.__support.common
    libc.src.__support.macros.config
)

add_entrypoint_object(
  gnu_get_libc_release
  SRCS
    gnu_get_libc_release.cpp
  HDRS
    gnu_get_libc_release.h
  DEPENDS
    libc.src.__support.common
    libc.src.__support.macros.config
)

add_entrypoint_object(
  progname
  SRCS
    progname.cpp
  HDRS
    progname.h
  DEPENDS
    libc.src.__support.common
    libc.src.__support.macros.config
)

add_entrypoint_object(
  __libc_current_sigrtmin
  SRCS
    __libc_current_sigrtmin.cpp
  HDRS
    __libc_current_sigrtmin.h
  DEPENDS
    libc.hdr.signal_macros
    libc.src.__support.common
    libc.src.__support.macros.config
)

add_entrypoint_object(
  __libc_current_sigrtmax
  SRCS
    __libc_current_sigrtmax.cpp
  HDRS
    __libc_current_sigrtmax.h
  DEPENDS
    libc.hdr.signal_macros
    libc.src.__support.common
    libc.src.__support.macros.config
)

add_entrypoint_object(
  __ctype_get_mb_cur_max
  SRCS
    __ctype_get_mb_cur_max.cpp
  HDRS
    __ctype_get_mb_cur_max.h
  DEPENDS
    libc.hdr.types.size_t
    libc.src.__support.common
    libc.src.__support.macros.config
    libc.src.stdlib.mb_cur_max
)

add_entrypoint_object(
  __sched_cpucount
  SRCS
    __sched_cpucount.cpp
  HDRS
    __sched_cpucount.h
  DEPENDS
    libc.hdr.types.cpu_set_t
    libc.hdr.types.size_t
    libc.src.__support.common
    libc.src.__support.macros.config
    libc.src.sched.__sched_getcpucount
)

add_entrypoint_object(
  __fpclassifyf
  SRCS
    __fpclassifyf.cpp
  HDRS
    __fpclassifyf.h
  DEPENDS
    libc.hdr.math_macros
    libc.src.__support.FPUtil.fp_bits
    libc.src.__support.common
    libc.src.__support.macros.config
)

add_entrypoint_object(
  __llvm_libc_init_main_thread
  SRCS
    __llvm_libc_init_main_thread.cpp
  HDRS
    __llvm_libc_init_main_thread.h
  DEPENDS
    libc.hdr.pthread_macros
    libc.hdr.sys_auxv_macros
    libc.src.__support.OSUtil.linux.syscall_wrappers.gettid
    libc.src.__support.common
    libc.src.__support.macros.config
    libc.src.__support.threads.tcb
    libc.src.__support.threads.robust_list
    libc.src.__support.threads.thread
)

add_header_library(
  program_name
  HDRS
    program_name.h
  DEPENDS
    libc.src.__support.macros.attributes
    libc.src.__support.macros.config
    libc.src.compat.progname
    libc.src.errno.program_invocation_name
    libc.src.errno.program_invocation_short_name
)

add_entrypoint_object(
  __llvm_libc_early_init
  SRCS
    __llvm_libc_early_init.cpp
  HDRS
    __llvm_libc_early_init.h
  DEPENDS
    libc.src.__support.OSUtil.linux.auxv
    libc.src.__support.common
    libc.src.__support.macros.config
    libc.src.unistd.environ
    .__llvm_libc_init_main_thread
    .program_name
)
