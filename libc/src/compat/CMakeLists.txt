# Names the platform ABI settled on that are not what the standards call the
# functions behind them. Code already built, and code that states its own
# declarations, asks for these.

add_entrypoint_object(
  __errno_location
  SRCS
    __errno_location.cpp
  HDRS
    __errno_location.h
  DEPENDS
    libc.src.__support.common
    libc.src.__support.libc_errno
    libc.src.__support.macros.config
)

add_entrypoint_object(
  syscall
  SRCS
    syscall.cpp
  HDRS
    syscall.h
  DEPENDS
    libc.hdr.stdint_proxy
    libc.src.__support.OSUtil.osutil
    libc.src.__support.common
    libc.src.__support.libc_errno
    libc.src.__support.macros.config
)

add_entrypoint_object(
  __xpg_strerror_r
  SRCS
    __xpg_strerror_r.cpp
  HDRS
    __xpg_strerror_r.h
  DEPENDS
    libc.hdr.errno_macros
    libc.hdr.types.size_t
    libc.src.__support.StringUtil.error_to_string
    libc.src.__support.common
    libc.src.__support.macros.config
    libc.src.string.memory_utils.inline_memcpy
)

add_entrypoint_object(
  lfs64
  SRCS
    lfs64.cpp
  HDRS
    lfs64.h
  DEPENDS
    libc.hdr.types.mode_t
    libc.hdr.types.off_t
    libc.hdr.types.size_t
    libc.hdr.types.struct_stat
    libc.src.__support.common
    libc.src.__support.macros.config
    libc.src.fcntl.open
    libc.src.sys.mman.mmap
    libc.src.sys.stat.fstat
    libc.src.sys.stat.lstat
    libc.src.sys.stat.stat
    libc.src.unistd.lseek
)
