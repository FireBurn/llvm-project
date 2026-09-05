add_custom_target(libc_gshadow_unittests)

if(NOT TARGET libc.src.gshadow.gshadow_utils)
  return()
endif()

add_libc_test(
  gshadow_utils_test
  SUITE
    libc_gshadow_unittests
  SRCS
    gshadow_utils_test.cpp
  DEPENDS
    libc.hdr.types.struct_sgrp
    libc.src.__support.CPP.span
    libc.src.gshadow.gshadow_utils
)
