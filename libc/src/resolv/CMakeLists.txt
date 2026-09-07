if(NOT TARGET libc.src.netdb.resolv.dns_message)
  return()
endif()

add_entrypoint_object(
  dn_expand
  SRCS
    dn_expand.cpp
  HDRS
    dn_expand.h
  DEPENDS
    libc.src.__support.common
    libc.src.__support.macros.config
    libc.src.netdb.resolv.dns_message
)

add_entrypoint_object(
  dn_comp
  SRCS
    dn_comp.cpp
  HDRS
    dn_comp.h
  DEPENDS
    libc.src.__support.common
    libc.src.__support.macros.config
)
