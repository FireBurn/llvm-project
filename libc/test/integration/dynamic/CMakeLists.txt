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

# Builds one such program from a single source and adds it to the integration
# tests.
function(add_libc_dynamic_test name)
  cmake_parse_arguments(ARG "" "" "SRCS;COMPILE_OPTIONS;DEPENDS" ${ARGN})
  add_executable(${name} ${ARG_SRCS})
  target_include_directories(${name} SYSTEM PRIVATE ${LIBC_INCLUDE_DIR})
  target_compile_options(${name} PRIVATE
    -fPIE -ffreestanding -fno-exceptions -fno-rtti ${ARG_COMPILE_OPTIONS})
  target_link_options(${name} PRIVATE
    -nostdlib -pie
    -Wl,--dynamic-linker=$<TARGET_FILE:libc-loader>
    -Wl,-rpath,$<TARGET_FILE_DIR:libc-shared>)
  target_link_libraries(${name} PRIVATE
    $<TARGET_OBJECTS:libc.startup.${LIBC_TARGET_OS}.crt1>
    libc-shared)
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
