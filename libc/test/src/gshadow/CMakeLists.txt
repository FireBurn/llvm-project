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
    libc.src.gshadow.sgetsgent
    libc.src.gshadow.sgetsgent_r
)

add_libc_test(
  gshadow_test
  SUITE
    libc_gshadow_unittests
  SRCS
    gshadow_test.cpp
  DEPENDS
    libc.hdr.errno_macros
    libc.hdr.types.FILE
    libc.hdr.types.struct_sgrp
    libc.src.gshadow.endsgent
    libc.src.gshadow.fgetsgent
    libc.src.gshadow.fgetsgent_r
    libc.src.gshadow.getsgent
    libc.src.gshadow.getsgent_r
    libc.src.gshadow.getsgnam
    libc.src.gshadow.getsgnam_r
    libc.src.gshadow.gshadow_utils
    libc.src.gshadow.putsgent
    libc.src.gshadow.setsgent
    libc.src.stdio.fclose
    libc.src.stdio.fopen
    libc.src.stdio.fwrite
    libc.src.stdio.remove
    libc.test.UnitTest.ErrnoCheckingTest
)
