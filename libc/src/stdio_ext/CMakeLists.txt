# Every one of these is a thin accessor on FILE. They exist so that code which
# would otherwise reach into the structure, gnulib above all, can ask instead.
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${LIBC_TARGET_OS})
  add_subdirectory(${LIBC_TARGET_OS})
endif()
add_subdirectory(generic)

function(add_stdio_ext_entrypoint_object name)
  if(TARGET libc.src.stdio_ext.${LIBC_TARGET_OS}.${name})
    add_entrypoint_object(
      ${name}
      ALIAS
      DEPENDS
        .${LIBC_TARGET_OS}.${name}
    )
  elseif(TARGET libc.src.stdio_ext.generic.${name})
    add_entrypoint_object(
      ${name}
      ALIAS
      DEPENDS
        .generic.${name}
    )
  endif()
endfunction()

add_stdio_ext_entrypoint_object(__fbufsize)
add_stdio_ext_entrypoint_object(__fpending)
add_stdio_ext_entrypoint_object(__freadahead)
add_stdio_ext_entrypoint_object(__fseterr)
add_stdio_ext_entrypoint_object(__freading)
add_stdio_ext_entrypoint_object(__fwriting)
add_stdio_ext_entrypoint_object(__freadable)
add_stdio_ext_entrypoint_object(__fwritable)
add_stdio_ext_entrypoint_object(__flbf)
add_stdio_ext_entrypoint_object(__fpurge)
