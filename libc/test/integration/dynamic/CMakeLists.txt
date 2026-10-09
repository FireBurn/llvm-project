# Programs linked against the shared libc and started by its loader. The
# dlfcn calls refuse to work in a static program, so they can only be tested
# this way.
if(NOT LIBC_ENABLE_SHARED OR NOT TARGET libc-loader)
  return()
endif()

add_library(libc-test-dl-object SHARED dl_object.cpp)
target_compile_options(libc-test-dl-object PRIVATE
  -fPIC -fno-exceptions -fno-rtti)
target_link_options(libc-test-dl-object PRIVATE -nostdlib
  -Wl,--version-script=${CMAKE_CURRENT_SOURCE_DIR}/dl_object.map)
set_target_properties(libc-test-dl-object PROPERTIES
  LINK_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/dl_object.map)

# Takes a mutex from a constructor, so it needs the library.
add_library(libc-test-dl-lock-object SHARED dl_lock_object.cpp)
target_include_directories(libc-test-dl-lock-object SYSTEM PRIVATE
  ${LIBC_INCLUDE_DIR})
target_compile_options(libc-test-dl-lock-object PRIVATE
  -fPIC -ffreestanding -fno-exceptions -fno-rtti)
target_link_options(libc-test-dl-lock-object PRIVATE -nostdlib
  -Wl,--dynamic-linker=$<TARGET_FILE:libc-loader>)
target_link_libraries(libc-test-dl-lock-object PRIVATE libc-shared)
add_dependencies(libc-test-dl-lock-object libc-headers)

# DT_INIT and DT_FINI come from -init and -fini; nothing else asks for them.
add_library(libc-test-dl-init-object SHARED dl_init_object.cpp)
target_compile_options(libc-test-dl-init-object PRIVATE
  -fPIC -fno-exceptions -fno-rtti)
# The program defines the symbols this records into.
target_link_options(libc-test-dl-init-object PRIVATE -nostdlib
  -Wl,-init=llvm_libc_dl_init -Wl,-fini=llvm_libc_dl_fini
  -Wl,--unresolved-symbols=ignore-all)

# Reports from its fini array that it ran.
add_library(libc-test-dl-exit-object SHARED dl_exit_object.cpp)
target_compile_options(libc-test-dl-exit-object PRIVATE
  -fPIC -fno-exceptions -fno-rtti)
target_link_options(libc-test-dl-exit-object PRIVATE -nostdlib
  -Wl,--unresolved-symbols=ignore-all)

# B needs A. The programs below load A before B, which a walk of the load
# order backwards would initialise the wrong way round.
add_library(libc-test-dl-order-a SHARED dl_order_a.cpp)
target_compile_options(libc-test-dl-order-a PRIVATE
  -fPIC -fno-exceptions -fno-rtti)
target_link_options(libc-test-dl-order-a PRIVATE -nostdlib)
add_library(libc-test-dl-order-b SHARED dl_order_b.cpp)
target_compile_options(libc-test-dl-order-b PRIVATE
  -fPIC -fno-exceptions -fno-rtti)
target_link_options(libc-test-dl-order-b PRIVATE -nostdlib
  -Wl,--unresolved-symbols=ignore-all)
target_link_libraries(libc-test-dl-order-b PRIVATE libc-test-dl-order-a)
add_library(libc-test-dl-order-x SHARED dl_order_x.cpp)
target_compile_options(libc-test-dl-order-x PRIVATE
  -fPIC -fno-exceptions -fno-rtti)
target_link_options(libc-test-dl-order-x PRIVATE -nostdlib)
# Given by file, as for the program below, so that A comes first.
target_link_options(libc-test-dl-order-x PRIVATE
  $<TARGET_FILE:libc-test-dl-order-a> $<TARGET_FILE:libc-test-dl-order-b>
  -Wl,-rpath,$<TARGET_FILE_DIR:libc-test-dl-order-a>)
add_dependencies(libc-test-dl-order-x
  libc-test-dl-order-a libc-test-dl-order-b)

# Reaches its thread local through a TLS descriptor, which only x86_64 has
# support for here.
if(LIBC_TARGET_ARCHITECTURE_IS_X86_64)
  add_library(libc-test-dl-tlsdesc-object SHARED dl_tlsdesc_object.cpp)
  target_compile_options(libc-test-dl-tlsdesc-object PRIVATE
    -fPIC -fno-exceptions -fno-rtti -mtls-dialect=gnu2)
  target_link_options(libc-test-dl-tlsdesc-object PRIVATE -nostdlib)
endif()

# Builds one such program from a single source and adds it to the integration
# tests.
function(add_libc_dynamic_test name)
  cmake_parse_arguments(ARG "NO_PIE" ""
    "SRCS;COMPILE_OPTIONS;LINK_OPTIONS;DEPENDS;LINK_LIBRARIES" ${ARGN})
  add_executable(${name} ${ARG_SRCS})
  target_include_directories(${name} SYSTEM PRIVATE ${LIBC_INCLUDE_DIR})
  # A program that is not position independent refers to data in a shared
  # object by an absolute address, which the linker satisfies with a copy
  # relocation.
  set(pie_compile -fPIE)
  set(pie_link -pie)
  if(ARG_NO_PIE)
    set(pie_compile -fno-pic)
    set(pie_link -no-pie)
  endif()
  target_compile_options(${name} PRIVATE
    ${pie_compile} -ffreestanding -fno-exceptions -fno-rtti
    ${ARG_COMPILE_OPTIONS})
  target_link_options(${name} PRIVATE
    -nostdlib ${pie_link} ${ARG_LINK_OPTIONS}
    -Wl,--dynamic-linker=$<TARGET_FILE:libc-loader>
    -Wl,-rpath,$<TARGET_FILE_DIR:libc-shared>)
  target_link_libraries(${name} PRIVATE
    $<TARGET_OBJECTS:libc.startup.${LIBC_TARGET_OS}.crt1>
    libc-shared ${ARG_LINK_LIBRARIES})
  if(LIBC_COMPILER_RT_BUILTINS)
    target_link_libraries(${name} PRIVATE ${LIBC_COMPILER_RT_BUILTINS})
  endif()
  add_dependencies(${name} libc-headers libc-loader libc-startup
    ${ARG_DEPENDS})
  add_custom_target(libc.test.integration.dynamic.${name}
    COMMAND $<TARGET_FILE:${name}>
    DEPENDS ${name}
    COMMENT "Running ${name}")
  add_dependencies(libc-integration-tests
    libc.test.integration.dynamic.${name})
endfunction()

add_libc_dynamic_test(
  dlopen_test
  SRCS
    dlopen_test.cpp
  COMPILE_OPTIONS
    -DLIBC_TEST_DL_OBJECT="$<TARGET_FILE:libc-test-dl-object>"
  DEPENDS
    libc-test-dl-object
)

add_libc_dynamic_test(
  dl_iterate_phdr_test
  SRCS
    dl_iterate_phdr_test.cpp
  COMPILE_OPTIONS
    -DLIBC_TEST_DL_OBJECT="$<TARGET_FILE:libc-test-dl-object>"
  DEPENDS
    libc-test-dl-object
)

add_libc_dynamic_test(
  dladdr_test
  SRCS
    dladdr_test.cpp
  COMPILE_OPTIONS
    -DLIBC_TEST_DL_OBJECT="$<TARGET_FILE:libc-test-dl-object>"
  DEPENDS
    libc-test-dl-object
)

add_libc_dynamic_test(
  dlsym_test
  SRCS
    dlsym_test.cpp
  COMPILE_OPTIONS
    -DLIBC_TEST_DL_OBJECT="$<TARGET_FILE:libc-test-dl-object>"
  DEPENDS
    libc-test-dl-object
)

add_libc_dynamic_test(
  copy_reloc_test
  NO_PIE
  SRCS
    copy_reloc_test.cpp
  LINK_LIBRARIES
    libc-test-dl-object
  DEPENDS
    libc-test-dl-object
)

add_libc_dynamic_test(
  init_fini_test
  SRCS
    init_fini_test.cpp
  COMPILE_OPTIONS
    -DLIBC_TEST_DL_INIT_OBJECT="$<TARGET_FILE:libc-test-dl-init-object>"
  LINK_OPTIONS
    -Wl,--export-dynamic
  DEPENDS
    libc-test-dl-init-object
)

add_libc_dynamic_test(
  exit_fini_test
  SRCS
    exit_fini_test.cpp
  COMPILE_OPTIONS
    -DLIBC_TEST_DL_EXIT_OBJECT="$<TARGET_FILE:libc-test-dl-exit-object>"
  LINK_OPTIONS
    -Wl,--export-dynamic
  DEPENDS
    libc-test-dl-exit-object
)

add_library(libc-test-dl-exit-linked SHARED dl_exit_object.cpp)
target_compile_options(libc-test-dl-exit-linked PRIVATE
  -fPIC -fno-exceptions -fno-rtti)
target_link_options(libc-test-dl-exit-linked PRIVATE -nostdlib
  -Wl,--unresolved-symbols=ignore-all)

add_libc_dynamic_test(
  exit_fini_linked_test
  SRCS
    exit_fini_test.cpp
  COMPILE_OPTIONS
    -DLIBC_TEST_LINKED
  LINK_OPTIONS
    -Wl,--export-dynamic
  LINK_LIBRARIES
    libc-test-dl-exit-linked
  DEPENDS
    libc-test-dl-exit-linked
)

add_libc_dynamic_test(
  init_order_test
  SRCS
    init_order_test.cpp
  LINK_OPTIONS
    -Wl,--export-dynamic
    # Given by file, since CMake puts a library after the ones that need it
    # when they are named as libraries, and this wants A first.
    $<TARGET_FILE:libc-test-dl-order-a>
    $<TARGET_FILE:libc-test-dl-order-b>
    -Wl,-rpath,$<TARGET_FILE_DIR:libc-test-dl-order-a>
  DEPENDS
    libc-test-dl-order-a
    libc-test-dl-order-b
)

add_libc_dynamic_test(
  init_order_dlopen_test
  SRCS
    init_order_test.cpp
  COMPILE_OPTIONS
    -DLIBC_TEST_DLOPEN
    -DLIBC_TEST_DL_ORDER_OBJECT="$<TARGET_FILE:libc-test-dl-order-x>"
  LINK_OPTIONS
    -Wl,--export-dynamic
  DEPENDS
    libc-test-dl-order-x
)

if(LIBC_TARGET_ARCHITECTURE_IS_X86_64)
  add_libc_dynamic_test(
    tlsdesc_test
    SRCS
      tlsdesc_test.cpp
    LINK_LIBRARIES
      libc-test-dl-tlsdesc-object
    DEPENDS
      libc-test-dl-tlsdesc-object
  )

  add_libc_dynamic_test(
    tlsdesc_dlopen_test
    SRCS
      tlsdesc_test.cpp
    COMPILE_OPTIONS
      -DLIBC_TEST_DLOPEN
      -DLIBC_TEST_DL_TLSDESC_OBJECT="$<TARGET_FILE:libc-test-dl-tlsdesc-object>"
    DEPENDS
      libc-test-dl-tlsdesc-object
  )
endif()

add_libc_dynamic_test(
  lock_in_init_test
  SRCS
    lock_in_init_test.cpp
  LINK_LIBRARIES
    libc-test-dl-lock-object
  DEPENDS
    libc-test-dl-lock-object
)

add_libc_dynamic_test(
  dlinfo_test
  SRCS
    dlinfo_test.cpp
  COMPILE_OPTIONS
    -DLIBC_TEST_DL_OBJECT="$<TARGET_FILE:libc-test-dl-object>"
  DEPENDS
    libc-test-dl-object
)

add_libc_dynamic_test(
  dlvsym_test
  SRCS
    dlvsym_test.cpp
  COMPILE_OPTIONS
    -DLIBC_TEST_DL_OBJECT="$<TARGET_FILE:libc-test-dl-object>"
  DEPENDS
    libc-test-dl-object
)

add_libc_dynamic_test(
  dladdr1_test
  SRCS
    dladdr1_test.cpp
  COMPILE_OPTIONS
    -DLIBC_TEST_DL_OBJECT="$<TARGET_FILE:libc-test-dl-object>"
  DEPENDS
    libc-test-dl-object
)
