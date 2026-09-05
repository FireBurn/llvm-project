if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${LIBC_TARGET_ARCHITECTURE})
  add_subdirectory(${LIBC_TARGET_ARCHITECTURE})
endif()

add_entrypoint_object(
  getcontext
  ALIAS
  DEPENDS
    .${LIBC_TARGET_ARCHITECTURE}.getcontext
)

add_entrypoint_object(
  setcontext
  ALIAS
  DEPENDS
    .${LIBC_TARGET_ARCHITECTURE}.setcontext
)

add_entrypoint_object(
  makecontext
  ALIAS
  DEPENDS
    .${LIBC_TARGET_ARCHITECTURE}.makecontext
)

add_entrypoint_object(
  swapcontext
  SRCS
    swapcontext.cpp
  HDRS
    swapcontext.h
  DEPENDS
    .getcontext
    .setcontext
    libc.include.llvm-libc-types.ucontext_t
    libc.src.errno.errno
)
