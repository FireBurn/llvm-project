if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${LIBC_TARGET_OS})
  add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/${LIBC_TARGET_OS})
endif()

add_entrypoint_object(
  mount
  ALIAS
  DEPENDS
    .${LIBC_TARGET_OS}.mount
)

add_entrypoint_object(
  umount
  ALIAS
  DEPENDS
    .${LIBC_TARGET_OS}.umount
)

add_entrypoint_object(
  umount2
  ALIAS
  DEPENDS
    .${LIBC_TARGET_OS}.umount2
)

add_entrypoint_object(
  fsopen
  ALIAS
  DEPENDS
    .${LIBC_TARGET_OS}.fsopen
)

add_entrypoint_object(
  fsmount
  ALIAS
  DEPENDS
    .${LIBC_TARGET_OS}.fsmount
)

add_entrypoint_object(
  fsconfig
  ALIAS
  DEPENDS
    .${LIBC_TARGET_OS}.fsconfig
)

add_entrypoint_object(
  fspick
  ALIAS
  DEPENDS
    .${LIBC_TARGET_OS}.fspick
)

add_entrypoint_object(
  open_tree
  ALIAS
  DEPENDS
    .${LIBC_TARGET_OS}.open_tree
)

add_entrypoint_object(
  move_mount
  ALIAS
  DEPENDS
    .${LIBC_TARGET_OS}.move_mount
)

add_entrypoint_object(
  mount_setattr
  ALIAS
  DEPENDS
    .${LIBC_TARGET_OS}.mount_setattr
)

add_entrypoint_object(
  pivot_root
  ALIAS
  DEPENDS
    .${LIBC_TARGET_OS}.pivot_root
)
