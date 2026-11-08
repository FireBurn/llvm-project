# This function merges multiple objects into a single relocatable object
#                     cc -r obj1.o obj2.o -o obj.o
# A relocatable object is an object file that is not fully linked into an
# executable or a shared library. It is an intermediate file format that can
# be passed into the linker.
# A crt object has arch-specific code and arch-agnostic code. To reduce code
# duplication, the implementation is split into multiple units. As a result,
# we need to merge them into a single relocatable object.
# See also:  https://maskray.me/blog/2022-11-21-relocatable-linking
function(merge_relocatable_object name)
  set(obj_list "")
  set(fq_link_libraries "")
  get_fq_deps_list(fq_dep_list ${ARGN})
  foreach(target IN LISTS fq_dep_list)
    list(APPEND obj_list "$<TARGET_OBJECTS:${target}>")
    get_target_property(libs ${target} DEPS)
    list(APPEND fq_link_libraries "${libs}")
  endforeach()
  list(REMOVE_DUPLICATES obj_list)
  list(REMOVE_DUPLICATES fq_link_libraries)
  get_fq_target_name(${name} fq_name)
  set(relocatable_target "${fq_name}.__relocatable__")
  add_executable(
    ${relocatable_target}
    ${obj_list}
  )
  # Pass -r to the driver is much cleaner than passing -Wl,-r: the compiler knows it is
  # a relocatable linking and will not pass other irrelevant flags to the linker.
  set(link_opts -r -nostdlib)
  if(explicit_target_triple AND LLVM_ENABLE_LLD)
    list(APPEND link_opts --target=${explicit_target_triple})
  endif()
  target_link_options(${relocatable_target} PRIVATE ${link_opts})
  set_target_properties(
    ${relocatable_target}
    PROPERTIES
      RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
      OUTPUT_NAME ${name}.o
  )
  add_library(${fq_name} OBJECT IMPORTED GLOBAL)
  add_dependencies(${fq_name} ${relocatable_target})
  target_link_libraries(${fq_name} INTERFACE ${fq_link_libraries})
  set_target_properties(
    ${fq_name}
    PROPERTIES
      LINKER_LANGUAGE CXX
      IMPORTED_OBJECTS ${CMAKE_CURRENT_BINARY_DIR}/${name}.o
      TARGET_TYPE ${OBJECT_LIBRARY_TARGET_TYPE}
      DEPS "${fq_link_libraries}"
  )
endfunction()

function(add_startup_object name)
  cmake_parse_arguments(
    "ADD_STARTUP_OBJECT"
    "" # Option argument
    "SRC" # Single value arguments
    "DEPENDS;COMPILE_OPTIONS" # Multi value arguments
    ${ARGN}
  )

  get_fq_target_name(${name} fq_target_name)

  add_object_library(
    ${name}
    SRCS ${ADD_STARTUP_OBJECT_SRC}
    DEPENDS ${ADD_STARTUP_OBJECT_DEPENDS}
    COMPILE_OPTIONS ${ADD_STARTUP_OBJECT_COMPILE_OPTIONS}
  )
  set_target_properties(
    ${fq_target_name}
    PROPERTIES
      OUTPUT_NAME ${name}.o
  )
endfunction()

check_cxx_compiler_flag("-r" LIBC_LINKER_SUPPORTS_RELOCATABLE)

if(NOT LIBC_LINKER_SUPPORTS_RELOCATABLE)
  message(STATUS "Skipping startup for target architecture ${LIBC_TARGET_ARCHITECTURE}: linker does not support -r")
  return()
endif()

if(NOT (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${LIBC_TARGET_ARCHITECTURE}))
  message(STATUS "Skipping startup for target architecture ${LIBC_TARGET_ARCHITECTURE}")
  return()
endif()

add_subdirectory(${LIBC_TARGET_ARCHITECTURE})

add_object_library(
  app
  SRCS
    app.cpp
  DEPENDS
    libc.config.app_h
    libc.src.__support.macros.config
)

add_object_library(
  gnu_property_section
  SRCS
    gnu_property_section.cpp
  HDRS
    gnu_property_section.h
  DEPENDS
    libc.src.__support.CPP.string_view
    libc.src.__support.macros.config
    libc.hdr.elf_proxy
    libc.hdr.link_macros
)

add_object_library(
  do_start
  SRCS
    do_start.cpp
  HDRS
    do_start.h
    irelative.h
    relocate_static_pie.h
  DEPENDS
    .app
    libc.config.app_h
    libc.hdr.elf_proxy
    libc.hdr.link_macros
    libc.hdr.pthread_macros
    libc.hdr.stdint_proxy
    libc.hdr.sys_auxv_macros
    libc.hdr.sys_mman_macros
    libc.hdr.types.struct_link_map
    libc.hdr.types.struct_r_debug
    libc.include.sys_syscall
    libc.src.__support.OSUtil.linux.auxv
    libc.src.__support.OSUtil.linux.syscall_wrappers.exit_group
    libc.src.__support.OSUtil.osutil
    libc.src.__support.elf.module
    libc.src.__support.elf.passive_abi
    libc.src.__support.threads.tcb
    libc.src.__support.threads.robust_list
    libc.src.__support.threads.thread
    libc.src.__support.threads.linux.futex_utils
    libc.src.__support.threads.linux.tls
    libc.src.__support.macros.config
    libc.startup.linux.${LIBC_TARGET_ARCHITECTURE}.irelative
    libc.src.link._r_debug
    libc.src.stdlib.exit
    libc.src.stdlib.atexit
    libc.src.unistd.environ
    libc.src.compat.program_name
    libc.src.android.debuggerd_hook
  COMPILE_OPTIONS
    -ffreestanding # To avoid compiler warnings about calling the main function.
    -fno-builtin # avoid emit unexpected calls
    -fno-stack-protector # stack protect canary is not available yet.
)

add_object_library(
  do_start_static_pie
  SRCS
    do_start.cpp
  HDRS
    do_start.h
    irelative.h
    relocate_static_pie.h
  DEPENDS
    .app
    libc.config.app_h
    libc.hdr.elf_macros
    libc.hdr.sys_auxv_macros
    libc.hdr.elf_proxy
    libc.hdr.link_macros
    libc.hdr.pthread_macros
    libc.hdr.stdint_proxy
    libc.hdr.sys_auxv_macros
    libc.hdr.sys_mman_macros
    libc.hdr.types.struct_link_map
    libc.hdr.types.struct_r_debug
    libc.include.sys_syscall
    libc.src.__support.OSUtil.linux.auxv
    libc.src.__support.OSUtil.linux.syscall_wrappers.exit_group
    libc.src.__support.OSUtil.osutil
    libc.src.__support.elf.module
    libc.src.__support.elf.passive_abi
    libc.src.__support.threads.tcb
    libc.src.__support.threads.robust_list
    libc.src.__support.threads.thread
    libc.src.__support.threads.linux.futex_utils
    libc.src.__support.threads.linux.tls
    libc.src.__support.macros.attributes
    libc.src.__support.macros.config
    libc.startup.linux.${LIBC_TARGET_ARCHITECTURE}.irelative
    libc.src.link._r_debug
    libc.src.stdlib.exit
    libc.src.stdlib.atexit
    libc.src.unistd.environ
    libc.src.compat.program_name
    libc.src.android.debuggerd_hook
  COMPILE_OPTIONS
    -ffreestanding # To avoid compiler warnings about calling the main function.
    -fno-builtin # avoid emit unexpected calls
    -fno-stack-protector # stack protect canary is not available yet.
    -DLIBC_STARTUP_STATIC_PIE
)

# A program linked against libc.so cannot reach the TLS routines there, which
# are hidden, so a shared build carries its own copy of them in crt1.o. A
# static link takes them from crt1.o in the same way and leaves libc.a's.
set(crt1_tls "")
if(LIBC_ENABLE_SHARED)
  set(crt1_tls
    libc.src.__support.threads.linux.${LIBC_TARGET_ARCHITECTURE}.tls)
endif()

# TODO: factor out crt1 into multiple objects
merge_relocatable_object(
  crt1
  .${LIBC_TARGET_ARCHITECTURE}.start
  ${crt1_tls}
  .${LIBC_TARGET_ARCHITECTURE}.irelative
  .do_start
  .gnu_property_section
)

# Static PIEs start from rcrt1.o, which relocates the program before anything
# else. It needs position independent objects, so only a shared build has it.
if(LIBC_ENABLE_SHARED)
  merge_relocatable_object(
    rcrt1
    .${LIBC_TARGET_ARCHITECTURE}.start
    ${crt1_tls}
    .${LIBC_TARGET_ARCHITECTURE}.irelative
    .do_start_static_pie
    .gnu_property_section
  )
endif()

# These are linked into every program. Built with LTO they would be bitcode,
# which lld merges into the LTO unit of the program, and a program that splits
# its unit differently then fails to link.
add_startup_object(
  crti
  SRC
    crti.cpp
  COMPILE_OPTIONS
    -fno-lto
)

add_startup_object(
  crtn
  SRC
    crtn.cpp
  COMPILE_OPTIONS
    -fno-lto
)

add_custom_target(libc-startup)
set(startup_components crt1 crti crtn)
if(LIBC_ENABLE_SHARED)
  list(APPEND startup_components rcrt1)
endif()
foreach(target IN LISTS startup_components)
  set(fq_target_name libc.startup.linux.${target})
  add_dependencies(libc-startup ${fq_target_name})
  install(FILES $<TARGET_OBJECTS:${fq_target_name}>
    DESTINATION ${LIBC_INSTALL_LIBRARY_DIR}
    RENAME $<TARGET_PROPERTY:${fq_target_name},OUTPUT_NAME>
    COMPONENT libc)
endforeach()

add_object_library(
  passive_abi
  SRCS
    passive_abi.cpp
  DEPENDS
    libc.src.__support.elf.passive_abi
    libc.src.__support.macros.config
)

# Position independent executables start from Scrt1.o. The objects of a shared
# build are position independent already, so crt1.o serves under both names.
if(LIBC_ENABLE_SHARED)
  install(FILES $<TARGET_OBJECTS:libc.startup.linux.crt1>
    DESTINATION ${LIBC_INSTALL_LIBRARY_DIR}
    RENAME Scrt1.o
    COMPONENT libc)
endif()
