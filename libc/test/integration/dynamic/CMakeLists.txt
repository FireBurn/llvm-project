# Programs linked against the shared libc and started by its loader. The
# dlfcn calls refuse to work in a static program, so they can only be tested
# this way.
if(NOT LIBC_ENABLE_SHARED OR NOT TARGET libc-loader)
  return()
endif()

add_library(libc-test-dl-object SHARED dl_object.cpp)
target_compile_options(libc-test-dl-object PRIVATE
  -fPIC -fno-exceptions -fno-rtti)
target_link_options(libc-test-dl-object PRIVATE -nostdlib)

# DT_INIT and DT_FINI come from -init and -fini; nothing else asks for them.
add_library(libc-test-dl-init-object SHARED dl_init_object.cpp)
target_compile_options(libc-test-dl-init-object PRIVATE
  -fPIC -fno-exceptions -fno-rtti)
# The program defines the symbols this records into.
target_link_options(libc-test-dl-init-object PRIVATE -nostdlib
  -Wl,-init=llvm_libc_dl_init -Wl,-fini=llvm_libc_dl_fini
  -Wl,--unresolved-symbols=ignore-all)

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
