if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${LIBC_TARGET_OS})
  add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/${LIBC_TARGET_OS})
endif()

add_entrypoint_object(
  raise
  ALIAS
  DEPENDS
    .${LIBC_TARGET_OS}.raise
)

add_entrypoint_object(
  kill
  ALIAS
  DEPENDS
    .${LIBC_TARGET_OS}.kill
)

add_entrypoint_object(
  killpg
  ALIAS
  DEPENDS
    .${LIBC_TARGET_OS}.killpg
)

add_entrypoint_object(
  sigaction
  ALIAS
  DEPENDS
    .${LIBC_TARGET_OS}.sigaction
)

add_entrypoint_object(
  sigpending
  ALIAS
  DEPENDS
    .${LIBC_TARGET_OS}.sigpending
)

add_entrypoint_object(
  psignal
  SRCS
    psignal.cpp
  HDRS
    psignal.h
  DEPENDS
    libc.hdr.types.FILE
    libc.src.__support.CPP.string_view
    libc.src.__support.File.file
    libc.src.__support.StringUtil.signal_to_string
    libc.src.__support.common
    libc.src.__support.libc_errno
    libc.src.__support.macros.config
    libc.src.stdio.stderr
)

add_entrypoint_object(
  siginterrupt
  SRCS
    siginterrupt.cpp
  HDRS
    siginterrupt.h
  DEPENDS
    libc.hdr.signal_macros
    libc.hdr.types.struct_sigaction
    libc.src.__support.common
    libc.src.__support.error_or
    libc.src.__support.macros.config
    libc.src.errno.errno
    libc.src.signal.linux.signal_utils
)

add_entrypoint_object(
  sigaltstack
  ALIAS
  DEPENDS
    .${LIBC_TARGET_OS}.sigaltstack
)

add_entrypoint_object(
  sigprocmask
  ALIAS
  DEPENDS
    .${LIBC_TARGET_OS}.sigprocmask
)

add_entrypoint_object(
  sigsuspend
  ALIAS
  DEPENDS
    .${LIBC_TARGET_OS}.sigsuspend
)

add_entrypoint_object(
  pthread_sigmask
  ALIAS
  DEPENDS
    .${LIBC_TARGET_OS}.pthread_sigmask
)

add_entrypoint_object(
  pthread_kill
  ALIAS
  DEPENDS
    .${LIBC_TARGET_OS}.pthread_kill
)

add_entrypoint_object(
  sigemptyset
  ALIAS
  DEPENDS
    .${LIBC_TARGET_OS}.sigemptyset
)

add_entrypoint_object(
  sigaddset
  ALIAS
  DEPENDS
    .${LIBC_TARGET_OS}.sigaddset
)

add_entrypoint_object(
  signal
  ALIAS
  DEPENDS
    .${LIBC_TARGET_OS}.signal
)

add_entrypoint_object(
  sigfillset
  ALIAS
  DEPENDS
    .${LIBC_TARGET_OS}.sigfillset
)

add_entrypoint_object(
  sigdelset
  ALIAS
  DEPENDS
    .${LIBC_TARGET_OS}.sigdelset
)

add_entrypoint_object(
  sigismember
  ALIAS
  DEPENDS
    .${LIBC_TARGET_OS}.sigismember
)

add_entrypoint_object(
  sigtimedwait
  ALIAS
  DEPENDS
    .${LIBC_TARGET_OS}.sigtimedwait
)

add_entrypoint_object(
  sigwaitinfo
  ALIAS
  DEPENDS
    .${LIBC_TARGET_OS}.sigwaitinfo
)

add_entrypoint_object(
  sigwait
  ALIAS
  DEPENDS
    .${LIBC_TARGET_OS}.sigwait
)

add_entrypoint_object(
  sigisemptyset
  ALIAS
  DEPENDS
    .${LIBC_TARGET_OS}.sigisemptyset
)

add_entrypoint_object(
  sigorset
  ALIAS
  DEPENDS
    .${LIBC_TARGET_OS}.sigorset
)

add_entrypoint_object(
  sigandset
  ALIAS
  DEPENDS
    .${LIBC_TARGET_OS}.sigandset
)

add_entrypoint_object(
  sigqueue
  ALIAS
  DEPENDS
    .${LIBC_TARGET_OS}.sigqueue
)

add_entrypoint_object(
  tgkill
  ALIAS
  DEPENDS
    .${LIBC_TARGET_OS}.tgkill
)

add_entrypoint_object(
  pthread_sigqueue
  ALIAS
  DEPENDS
    .${LIBC_TARGET_OS}.pthread_sigqueue
)

add_entrypoint_object(
  sig2str
  SRCS
    sig2str.cpp
  HDRS
    sig2str.h
  DEPENDS
    libc.hdr.signal_macros
    libc.src.__support.CPP.string_view
    libc.src.__support.common
    libc.src.__support.macros.config
    libc.src.__support.macros.null_check
    libc.src.signal.signal_names
)

add_entrypoint_object(
  str2sig
  SRCS
    str2sig.cpp
  HDRS
    str2sig.h
  DEPENDS
    libc.hdr.signal_macros
    libc.src.__support.CPP.string_view
    libc.src.__support.common
    libc.src.__support.macros.config
    libc.src.__support.macros.null_check
    libc.src.signal.signal_names
)

add_header_library(
  signal_text
  HDRS
    signal_text.h
  DEPENDS
    libc.hdr.signal_macros
    libc.src.__support.StringUtil.platform_signals
    libc.src.__support.macros.config
    libc.src.signal.signal_names
)

add_entrypoint_object(
  sys_siglist
  SRCS
    sys_siglist.cpp
  HDRS
    sys_siglist.h
  DEPENDS
    libc.hdr.signal_macros
    libc.src.__support.common
    libc.src.__support.macros.config
    libc.src.signal.signal_text
)

add_entrypoint_object(
  sys_signame
  SRCS
    sys_signame.cpp
  HDRS
    sys_signame.h
  DEPENDS
    libc.hdr.signal_macros
    libc.src.__support.common
    libc.src.__support.macros.config
    libc.src.signal.signal_text
)

add_header_library(
  signal_names
  HDRS
    signal_names.h
  DEPENDS
    libc.hdr.signal_macros
    libc.hdr.types.size_t
    libc.src.__support.macros.attributes
    libc.src.__support.macros.config
)

add_entrypoint_object(
  pthread_sigmask64
  SRCS
    pthread_sigmask64.cpp
  HDRS
    pthread_sigmask64.h
  DEPENDS
    libc.hdr.types.sigset64_t
    libc.src.__support.common
    libc.src.__support.macros.config
    libc.src.signal.pthread_sigmask
)

add_entrypoint_object(
  sigaction64
  SRCS
    sigaction64.cpp
  HDRS
    sigaction64.h
  DEPENDS
    libc.hdr.types.sigset64_t
    libc.hdr.types.struct_sigaction
    libc.hdr.types.struct_sigaction64
    libc.src.__support.common
    libc.src.__support.macros.config
    libc.src.signal.sigaction
)

add_entrypoint_object(
  sigaddset64
  SRCS
    sigaddset64.cpp
  HDRS
    sigaddset64.h
  DEPENDS
    libc.hdr.types.sigset64_t
    libc.src.__support.common
    libc.src.__support.macros.config
    libc.src.signal.sigaddset
)

add_entrypoint_object(
  sigdelset64
  SRCS
    sigdelset64.cpp
  HDRS
    sigdelset64.h
  DEPENDS
    libc.hdr.types.sigset64_t
    libc.src.__support.common
    libc.src.__support.macros.config
    libc.src.signal.sigdelset
)

add_entrypoint_object(
  sigemptyset64
  SRCS
    sigemptyset64.cpp
  HDRS
    sigemptyset64.h
  DEPENDS
    libc.hdr.types.sigset64_t
    libc.src.__support.common
    libc.src.__support.macros.config
    libc.src.signal.sigemptyset
)

add_entrypoint_object(
  sigfillset64
  SRCS
    sigfillset64.cpp
  HDRS
    sigfillset64.h
  DEPENDS
    libc.hdr.types.sigset64_t
    libc.src.__support.common
    libc.src.__support.macros.config
    libc.src.signal.sigfillset
)

add_entrypoint_object(
  sigismember64
  SRCS
    sigismember64.cpp
  HDRS
    sigismember64.h
  DEPENDS
    libc.hdr.types.sigset64_t
    libc.src.__support.common
    libc.src.__support.macros.config
    libc.src.signal.sigismember
)

add_entrypoint_object(
  sigpending64
  SRCS
    sigpending64.cpp
  HDRS
    sigpending64.h
  DEPENDS
    libc.hdr.types.sigset64_t
    libc.src.__support.common
    libc.src.__support.macros.config
    libc.src.signal.sigpending
)

add_entrypoint_object(
  sigprocmask64
  SRCS
    sigprocmask64.cpp
  HDRS
    sigprocmask64.h
  DEPENDS
    libc.hdr.types.sigset64_t
    libc.src.__support.common
    libc.src.__support.macros.config
    libc.src.signal.sigprocmask
)

add_entrypoint_object(
  sigsuspend64
  SRCS
    sigsuspend64.cpp
  HDRS
    sigsuspend64.h
  DEPENDS
    libc.hdr.types.sigset64_t
    libc.src.__support.common
    libc.src.__support.macros.config
    libc.src.signal.sigsuspend
)

add_entrypoint_object(
  sigtimedwait64
  SRCS
    sigtimedwait64.cpp
  HDRS
    sigtimedwait64.h
  DEPENDS
    libc.hdr.types.siginfo_t
    libc.hdr.types.sigset64_t
    libc.hdr.types.struct_timespec
    libc.src.__support.common
    libc.src.__support.macros.config
    libc.src.signal.sigtimedwait
)

add_entrypoint_object(
  sigwait64
  SRCS
    sigwait64.cpp
  HDRS
    sigwait64.h
  DEPENDS
    libc.hdr.types.sigset64_t
    libc.src.__support.common
    libc.src.__support.macros.config
    libc.src.signal.sigwait
)

add_entrypoint_object(
  sigwaitinfo64
  SRCS
    sigwaitinfo64.cpp
  HDRS
    sigwaitinfo64.h
  DEPENDS
    libc.hdr.types.siginfo_t
    libc.hdr.types.sigset64_t
    libc.src.__support.common
    libc.src.__support.macros.config
    libc.src.signal.sigwaitinfo
)
