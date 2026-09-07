# Names the platform ABI settled on that are not what the standards call the
# functions behind them. Code already built, and code that states its own
# declarations, asks for these.

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
