add_libc_testsuite(libc-include-tests)

add_libc_test(
  resolv_state_layout_test
  SUITE
    libc-include-tests
  SRCS
    resolv_state_layout_test.cpp
  DEPENDS
    libc.include.resolv
)

add_libc_test(
  glibc_layouts_test
  SUITE
    libc-include-tests
  SRCS
    glibc_layouts_test.cpp
  DEPENDS
    libc.include.locale
    libc.include.regex
    libc.include.spawn
    libc.include.sys_statvfs
    libc.include.fenv
    libc.include.poll
)

add_libc_test(
  sys_stat_layout_test
  SUITE
    libc-include-tests
  SRCS
    sys_stat_layout_test.cpp
  DEPENDS
    libc.include.sys_stat
)

add_libc_test(
  pwd_size_t_test
  SUITE
    libc-include-tests
  SRCS
    pwd_size_t_test.cpp
  DEPENDS
    libc.include.pwd
)

add_libc_test(
  link_elf_types_test
  SUITE
    libc-include-tests
  SRCS
    link_elf_types_test.cpp
  DEPENDS
    libc.include.link
)

add_libc_test(
  link_dynamic_tags_test
  SUITE
    libc-include-tests
  SRCS
    link_dynamic_tags_test.cpp
  DEPENDS
    libc.include.link
)

add_libc_test(
  link_dt_flags_1_bits_test
  SUITE
    libc-include-tests
  SRCS
    link_dt_flags_1_bits_test.cpp
  DEPENDS
    libc.include.link
)

add_libc_test(
  signal_ucontext_t_test
  SUITE
    libc-include-tests
  SRCS
    signal_ucontext_t_test.cpp
  DEPENDS
    libc.include.signal
)

add_libc_test(
  setjmp_underscore_names_test
  SUITE
    libc-include-tests
  SRCS
    setjmp_underscore_names_test.cpp
  DEPENDS
    libc.include.setjmp
)

add_libc_test(
  string_strdupa_test
  SUITE
    libc-include-tests
  SRCS
    string_strdupa_test.cpp
  DEPENDS
    libc.include.string
)

add_libc_test(
  math_relational_test
  SUITE
    libc-include-tests
  SRCS
    math_relational_test.cpp
  DEPENDS
    libc.include.math
)

add_libc_test(
  math_iszero_templates_test
  SUITE
    libc-include-tests
  SRCS
    math_iszero_templates_test.cpp
  DEPENDS
    libc.include.math
)

add_libc_test(
  stdlib_no_float_macros_test
  SUITE
    libc-include-tests
  SRCS
    stdlib_no_float_macros_test.cpp
  DEPENDS
)

add_libc_test(
  sys_time_conversions_test
  SUITE
    libc-include-tests
  SRCS
    sys_time_conversions_test.cpp
  DEPENDS
    libc.include.sys_time
)

add_libc_test(
  unistd_posix_timeouts_test
  SUITE
    libc-include-tests
  SRCS
    unistd_posix_timeouts_test.cpp
  DEPENDS
    libc.include.unistd
)

add_libc_test(
  sys_vt_test
  SUITE
    libc-include-tests
  SRCS
    sys_vt_test.cpp
  DEPENDS
    libc.include.sys_vt
)

add_libc_test(
  sys_ttydefaults_test
  SUITE
    libc-include-tests
  SRCS
    sys_ttydefaults_test.cpp
  DEPENDS
    libc.include.sys_ttydefaults
)

add_libc_test(
  sys_kd_test
  SUITE
    libc-include-tests
  SRCS
    sys_kd_test.cpp
  DEPENDS
    libc.include.sys_kd
)

add_libc_test(
  net_if_interface_flags_test
  SUITE
    libc-include-tests
  SRCS
    net_if_interface_flags_test.cpp
  DEPENDS
    libc.include.net_if
)

add_libc_test(
  net_ethernet_test
  SUITE
    libc-include-tests
  SRCS
    net_ethernet_test.cpp
  DEPENDS
    libc.include.net_ethernet
)

add_libc_test(
  netinet_in_tcp_socket_test
  SUITE
    libc-include-tests
  SRCS
    netinet_in_tcp_socket_test.cpp
  DEPENDS
    libc.include.netinet_in
)

add_libc_test(
  sys_errno_test
  SUITE
    libc-include-tests
  SRCS
    sys_errno_test.cpp
  DEPENDS
    libc.include.sys_errno
)

add_libc_test(
  paths_test
  SUITE
    libc-include-tests
  SRCS
    paths_test.cpp
  DEPENDS
    libc.include.paths
)

add_libc_test(
  sys_msg_ipc_info_test
  SUITE
    libc-include-tests
  SRCS
    sys_msg_ipc_info_test.cpp
  DEPENDS
    libc.include.sys_msg
)

add_libc_test(
  sys_ipc_carried_test
  SUITE
    libc-include-tests
  SRCS
    sys_ipc_carried_test.cpp
  DEPENDS
    libc.include.sys_msg
    libc.include.sys_sem
    libc.include.sys_shm
)

add_libc_test(
  utmpx_file_macros_test
  SUITE
    libc-include-tests
  SRCS
    utmpx_file_macros_test.cpp
  DEPENDS
    libc.include.utmpx
)

add_libc_test(
  time_linux_macros_test
  SUITE
    libc-include-tests
  SRCS
    time_linux_macros_test.cpp
  DEPENDS
    libc.include.time
)

add_libc_test(
  sys_mtio_test
  SUITE
    libc-include-tests
  SRCS
    sys_mtio_test.cpp
  DEPENDS
    libc.include.sys_mtio
)

add_libc_test(
  ar_test
  SUITE
    libc-include-tests
  SRCS
    ar_test.cpp
  DEPENDS
    libc.include.ar
)

add_libc_test(
  syslog_name_tables_test
  SUITE
    libc-include-tests
  SRCS
    syslog_name_tables_test.cpp
  DEPENDS
    libc.include.syslog
)

add_libc_test(
  stdbit_stdint_test
  SUITE
    libc-include-tests
  SRCS
    stdbit_stdint_test.cpp
  DEPENDS
    libc.include.stdbit
)

add_libc_test(
  sys_quota_uapi_test
  SUITE
    libc-include-tests
  SRCS
    sys_quota_uapi_test.cpp
  DEPENDS
    libc.include.sys_quota
)

add_libc_test(
  stdlib_wait_macros_test
  SUITE
    libc-include-tests
  SRCS
    stdlib_wait_macros_test.cpp
  DEPENDS
    libc.include.stdlib
)

add_libc_test(
  common_cxx98_spellings_test
  SUITE
    libc-include-tests
  SRCS
    common_cxx98_spellings_test.cpp
  DEPENDS
    libc.include.stdlib
)

add_libc_test(
  sys_types_fd_set_test
  SUITE
    libc-include-tests
  SRCS
    sys_types_fd_set_test.cpp
  DEPENDS
    libc.include.sys_types
)

add_libc_test(
  sys_types_bsd_integer_test
  SUITE
    libc-include-tests
  SRCS
    sys_types_bsd_integer_test.cpp
  DEPENDS
    libc.include.sys_types
)

add_libc_test(
  common_iso_10646_test
  SUITE
    libc-include-tests
  SRCS
    common_iso_10646_test.cpp
  DEPENDS
    libc.include.wchar
)

add_libc_test(
  sys_cdefs_test
  SUITE
    libc-include-tests
  SRCS
    sys_cdefs_test.cpp
  DEPENDS
)

add_libc_test(
  dirent_lfs64_test
  SUITE
    libc-include-tests
  SRCS
    dirent_lfs64_test.cpp
  DEPENDS
    libc.include.dirent
)

add_libc_test(
  lfs64_test
  SUITE
    libc-include-tests
  SRCS
    lfs64_test.cpp
  DEPENDS
    libc.include.fcntl
    libc.include.sys_stat
    libc.include.sys_types
    libc.include.unistd
)

add_libc_test(
  wait_sys_wait_test
  SUITE
    libc-include-tests
  SRCS
    wait_sys_wait_test.cpp
  DEPENDS
    libc.include.wait
)

add_libc_test(
  netpacket_packet_test
  SUITE
    libc-include-tests
  SRCS
    netpacket_packet_test.cpp
  DEPENDS
    libc.include.netpacket_packet
)

add_libc_test(
  netinet_in6_macros_test
  SUITE
    libc-include-tests
  SRCS
    netinet_in6_macros_test.cpp
  DEPENDS
    libc.include.netinet_in
)

add_libc_test(
  netdb_address_classes_test
  SUITE
    libc-include-tests
  SRCS
    netdb_address_classes_test.cpp
  DEPENDS
    libc.include.netdb
)

add_libc_test(
  netinet_in_systm_test
  SUITE
    libc-include-tests
  SRCS
    netinet_in_systm_test.cpp
  DEPENDS
    libc.include.netinet_in_systm
)

add_libc_test(
  netinet_in_ports_test
  SUITE
    libc-include-tests
  SRCS
    netinet_in_ports_test.cpp
  DEPENDS
    libc.include.netinet_in
)

add_libc_test(
  netinet_in_multicast_source_test
  SUITE
    libc-include-tests
  SRCS
    netinet_in_multicast_source_test.cpp
  DEPENDS
    libc.include.netinet_in
)

add_libc_test(
  netinet_in6_parts_test
  SUITE
    libc-include-tests
  SRCS
    netinet_in6_parts_test.cpp
  DEPENDS
    libc.include.netinet_in
)

add_libc_test(
  netinet_tcp_info_test
  SUITE
    libc-include-tests
  SRCS
    netinet_tcp_info_test.cpp
  DEPENDS
    libc.include.netinet_tcp
)

add_libc_test(
  arpa_nameser_accessors_test
  SUITE
    libc-include-tests
  SRCS
    arpa_nameser_accessors_test.cpp
  DEPENDS
    libc.include.arpa_nameser
)

add_libc_test(
  netinet_tcp_socket_test
  SUITE
    libc-include-tests
  SRCS
    netinet_tcp_socket_test.cpp
  DEPENDS
    libc.include.netinet_tcp
)

add_libc_test(
  netinet_ip_icmp_struct_test
  SUITE
    libc-include-tests
  SRCS
    netinet_ip_icmp_struct_test.cpp
  DEPENDS
    libc.include.netinet_ip_icmp
)

add_libc_test(
  netinet_ip6_extension_test
  SUITE
    libc-include-tests
  SRCS
    netinet_ip6_extension_test.cpp
  DEPENDS
    libc.include.netinet_ip6
)

add_libc_test(
  netinet_igmp_test
  SUITE
    libc-include-tests
  SRCS
    netinet_igmp_test.cpp
  DEPENDS
    libc.include.netinet_igmp
)

add_libc_test(
  net_if_socket_test
  SUITE
    libc-include-tests
  SRCS
    net_if_socket_test.cpp
  DEPENDS
    libc.include.net_if
)

add_libc_test(
  arpa_nameser_compat_test
  SUITE
    libc-include-tests
  SRCS
    arpa_nameser_compat_test.cpp
  DEPENDS
    libc.include.arpa_nameser_compat
)

add_libc_test(
  sys_wait_signal_test
  SUITE
    libc-include-tests
  SRCS
    sys_wait_signal_test.cpp
  DEPENDS
    libc.include.sys_wait
)

add_libc_test(
  gshadow_shadow_files_test
  SUITE
    libc-include-tests
  SRCS
    gshadow_shadow_files_test.cpp
  DEPENDS
    libc.include.gshadow
)

add_libc_test(
  lastlog_utmp_test
  SUITE
    libc-include-tests
  SRCS
    lastlog_utmp_test.cpp
  DEPENDS
    libc.include.lastlog
)

add_libc_test(
  values_test
  SUITE
    libc-include-tests
  SRCS
    values_test.cpp
  DEPENDS
    libc.include.values
)

# Its structures are written for x86_64 alone.
if(LIBC_TARGET_ARCHITECTURE_IS_X86_64)
  add_libc_test(
    sys_user_test
    SUITE
      libc-include-tests
    SRCS
      sys_user_test.cpp
    DEPENDS
      libc.include.sys_user
  )
endif()

add_libc_test(
  sys_soundcard_test
  SUITE
    libc-include-tests
  SRCS
    sys_soundcard_test.cpp
  DEPENDS
    libc.include.sys_soundcard
)

# Its structures are written for x86_64 alone.
if(LIBC_TARGET_ARCHITECTURE_IS_X86_64)
  add_libc_test(
    sys_procfs_test
    SUITE
      libc-include-tests
    SRCS
      sys_procfs_test.cpp
    DEPENDS
      libc.include.sys_procfs
  )
endif()

add_libc_test(
  syslog_path_log_test
  SUITE
    libc-include-tests
  SRCS
    syslog_path_log_test.cpp
  DEPENDS
    libc.include.syslog
)

add_libc_test(
  fcntl_access_modes_test
  SUITE
    libc-include-tests
  SRCS
    fcntl_access_modes_test.cpp
  DEPENDS
    libc.include.fcntl
)

add_libc_test(
  dirent_fileno_test
  SUITE
    libc-include-tests
  SRCS
    dirent_fileno_test.cpp
  DEPENDS
    libc.include.dirent
)

add_libc_test(
  sys_dir_dirent_test
  SUITE
    libc-include-tests
  SRCS
    sys_dir_dirent_test.cpp
  DEPENDS
    libc.include.sys_dir
)

add_libc_test(
  sys_socket_loff_t_test
  SUITE
    libc-include-tests
  SRCS
    sys_socket_loff_t_test.cpp
  DEPENDS
    libc.include.sys_socket
)

add_libc_test(
  link_em_machine_numbers_test
  SUITE
    libc-include-tests
  SRCS
    link_em_machine_numbers_test.cpp
  DEPENDS
    libc.include.link
)

add_libc_test(
  link_stb_gnu_unique_test
  SUITE
    libc-include-tests
  SRCS
    link_stb_gnu_unique_test.cpp
  DEPENDS
    libc.include.link
)

add_libc_test(
  signal_handler_values_test
  SUITE
    libc-include-tests
  SRCS
    signal_handler_values_test.cpp
  DEPENDS
    libc.include.signal
)

add_libc_test(
  pty_termios_test
  SUITE
    libc-include-tests
  SRCS
    pty_termios_test.cpp
  DEPENDS
    libc.include.pty
)

add_libc_test(
  signal_sigevent_layout_test
  SUITE
    libc-include-tests
  SRCS
    signal_sigevent_layout_test.cpp
  DEPENDS
    libc.include.signal
)

add_libc_test(
  stdlib_sys_types_test
  SUITE
    libc-include-tests
  SRCS
    stdlib_sys_types_test.cpp
  DEPENDS
    libc.include.stdlib
)

add_libc_test(
  alloca_test
  SUITE
    libc-include-tests
  SRCS
    alloca_test.cpp
  DEPENDS
    libc.include.llvm-libc-macros.alloca_macros
)

add_libc_test(
  assert_test
  SUITE
    libc-include-tests
  SRCS
    assert_test.cpp
  DEPENDS
    libc.include.llvm-libc-macros.assert_macros
)

add_libc_test(
  scsi_sg_test
  SUITE
    libc-include-tests
  SRCS
    scsi_sg_test.cpp
  DEPENDS
    libc.include.llvm-libc-macros.scsi_sg_macros
    libc.include.llvm-libc-types.struct_ccs_modesel_head
    libc.include.llvm-libc-types.struct_sg_header
    libc.include.llvm-libc-types.struct_sg_io_hdr
    libc.include.llvm-libc-types.struct_sg_iovec
    libc.include.llvm-libc-types.struct_sg_req_info
    libc.include.llvm-libc-types.struct_sg_scsi_id
)

add_libc_test(
  arpa_nameser_test
  SUITE
    libc-include-tests
  SRCS
    arpa_nameser_test.cpp
  DEPENDS
    libc.include.llvm-libc-macros.arpa_nameser_macros
    libc.include.llvm-libc-types.HEADER
    libc.src.string.memory_utils.inline_memcpy
)

add_libc_test(
  net_if_test
  SUITE
    libc-include-tests
  SRCS
    net_if_test.cpp
  DEPENDS
    libc.include.llvm-libc-types.struct_ifreq
)

add_libc_test(
  netinet_ip_icmp_test
  SUITE
    libc-include-tests
  SRCS
    netinet_ip_icmp_test.cpp
  DEPENDS
    libc.include.llvm-libc-macros.netinet_ip_icmp_macros
    libc.include.llvm-libc-types.struct_icmphdr
)

add_libc_test(
  net_route_test
  SUITE
    libc-include-tests
  SRCS
    net_route_test.cpp
  DEPENDS
    libc.include.llvm-libc-macros.net_route_macros
    libc.include.llvm-libc-types.struct_rtentry
    libc.include.llvm-libc-types.struct_in6_rtmsg
)

add_libc_test(
  net_if_arp_test
  SUITE
    libc-include-tests
  SRCS
    net_if_arp_test.cpp
  DEPENDS
    libc.include.llvm-libc-macros.net_if_arp_macros
    libc.include.llvm-libc-types.struct_arpreq
    libc.include.llvm-libc-types.struct_arpreq_old
)

add_libc_test(
  features_test
  SUITE
    libc-include-tests
  SRCS
    features_test.cpp
)

add_libc_test(
  features_c_test
  C_TEST
  SUITE
    libc-include-tests
  SRCS
    features_test.c
)

add_libc_test(
  sys_param_test
  SUITE
    libc-include-tests
  SRCS
    sys_param_test.cpp
  DEPENDS
    libc.include.llvm-libc-macros.sys_param_macros
    libc.include.llvm-libc-types.dev_t
)

add_libc_test(
  elf_test
  SUITE
    libc-include-tests
  SRCS
    elf_test.cpp
  DEPENDS
    libc.include.llvm-libc-macros.elf_macros
    libc.include.llvm-libc-types.Elf32_Move
    libc.include.llvm-libc-types.Elf32_Section
    libc.include.llvm-libc-types.Elf32_Sword
    libc.include.llvm-libc-types.Elf32_Word
    libc.include.llvm-libc-types.Elf64_Lib
    libc.include.llvm-libc-types.Elf64_Move
    libc.include.llvm-libc-types.Elf64_Section
    libc.include.llvm-libc-types.Elf64_Sword
    libc.include.llvm-libc-types.Elf64_Sxword
    libc.include.llvm-libc-types.Elf64_Syminfo
    libc.include.llvm-libc-types.Elf64_Xword
)

add_libc_test(
  sys_klog_test
  SUITE
    libc-include-tests
  SRCS
    sys_klog_test.cpp
  DEPENDS
    libc.include.sys_klog
)

add_libc_test(
  complex_test
  SUITE
    libc-include-tests
  SRCS
    complex_test.cpp
  DEPENDS
    libc.include.llvm-libc-macros.complex_macros
)

add_libc_test(
  unistd_macros_test
  SUITE
    libc-include-tests
  SRCS
    unistd_macros_test.cpp
  DEPENDS
    libc.include.llvm-libc-macros.unistd_macros
)
add_libc_test(
  sys_queue_test
  SUITE
    libc-include-tests
  SRCS
    sys/queue_test.cpp
  DEPENDS
    libc.include.llvm-libc-macros.sys_queue_macros
    libc.src.__support.char_vector
    libc.src.__support.CPP.string
  COMPILE_OPTIONS
    # This is needed because the __containerof macro uses statement expression.
    -Wno-gnu-statement-expression-from-macro-expansion
)

if(LIBC_TARGET_OS_IS_LINUX)
  add_libc_test(
    sys_time_macros_test
    SUITE
      libc-include-tests
    SRCS
      sys_time_macros_test.cpp
    DEPENDS
      libc.include.llvm-libc-macros.sys_time_macros
      libc.include.llvm-libc-types.struct_timeval
      libc.include.llvm-libc-types.suseconds_t
      libc.include.llvm-libc-types.time_t
  )
endif()

add_libc_test(
  sys_select_test
  SUITE
    libc-include-tests
  SRCS
    sys/select_test.cpp
  DEPENDS
    libc.include.llvm-libc-macros.sys_select_macros
    libc.include.llvm-libc-types.fd_mask
    libc.include.llvm-libc-types.fd_set
)

add_libc_test(
  sys_un_test
  SUITE
    libc-include-tests
  SRCS
    sys/un_test.cpp
  DEPENDS
    libc.include.llvm-libc-macros.sys_un_macros
    libc.include.llvm-libc-types.struct_sockaddr_un
)

add_libc_test(
  stdbit_test
  SUITE
    libc-include-tests
  HDRS
    stdbit_stub.h
  SRCS
    stdbit_test.cpp
  DEPENDS
    libc.include.llvm-libc-macros.stdbit_macros
    libc.include.llvm_libc_common_h
    libc.include.stdbit
  # Intentionally do not depend on libc.src.stdbit.*. The include test is
  # simply testing the macros provided by stdbit.h, not the implementation
  # of the underlying functions which the type generic macros may dispatch
  # to.
)
add_libc_test(
  stdbit_c_test
  C_TEST
  SUITE
    libc-include-tests
  HDRS
    stdbit_stub.h
  SRCS
    stdbit_test.c
  COMPILE_OPTIONS
    -Wall
    -Werror
  DEPENDS
    libc.include.llvm-libc-macros.stdbit_macros
    libc.include.llvm_libc_common_h
    libc.include.stdbit
    libc.src.assert.__assert_fail
  # Intentionally do not depend on libc.src.stdbit.*. The include test is
  # simply testing the macros provided by stdbit.h, not the implementation
  # of the underlying functions which the type generic macros may dispatch
  # to.
)

add_libc_test(
  stdckdint_test
  SUITE
    libc-include-tests
  SRCS
    stdckdint_test.cpp
  DEPENDS
    libc.include.llvm-libc-macros.stdckdint_macros
)

add_libc_test(
  sched_test
  SUITE
    libc-include-tests
  SRCS
    sched_test.cpp
  DEPENDS
    libc.include.sched
    libc.src.sched.sched_getscheduler
)

add_libc_test(
  issubnormal_test
  SUITE
    libc-include-tests
  SRCS
    issubnormal_test.cpp
  DEPENDS
    libc.include.llvm-libc-macros.math_function_macros
)

add_libc_test(
  issubnormalf_test
  SUITE
    libc-include-tests
  SRCS
    issubnormalf_test.cpp
  DEPENDS
    libc.include.llvm-libc-macros.math_function_macros
)

add_libc_test(
  issubnormall_test
  SUITE
    libc-include-tests
  SRCS
    issubnormall_test.cpp
  DEPENDS
    libc.include.llvm-libc-macros.math_function_macros
)

add_libc_test(
  isnormal_test
  SUITE
    libc-include-tests
  SRCS
    isnormal_test.cpp
  DEPENDS
    libc.include.llvm-libc-macros.math_function_macros
)

add_libc_test(
  isnormalf_test
  SUITE
    libc-include-tests
  SRCS
    isnormalf_test.cpp
  DEPENDS
    libc.include.llvm-libc-macros.math_function_macros
)

add_libc_test(
  isnormall_test
  SUITE
    libc-include-tests
  SRCS
    isnormall_test.cpp
  DEPENDS
    libc.include.llvm-libc-macros.math_function_macros
)

add_libc_test(
  fpclassify_test
  SUITE
    libc-include-tests
  SRCS
    fpclassify_test.cpp
  DEPENDS
    libc.include.llvm-libc-macros.math_function_macros
)

add_libc_test(
  float_test
  SUITE
    libc-include-tests
  SRCS
    float_test.cpp
  DEPENDS
    libc.include.llvm-libc-macros.float_macros
)

add_libc_test(
  stdint_constant_test
  SUITE
    libc-include-tests
  SRCS
    stdint_constant_test.cpp
  DEPENDS
    libc.include.llvm-libc-macros.limits_macros
    libc.include.llvm-libc-macros.stdint_macros
)

add_libc_test(
  fpclassifyf_test
  SUITE
    libc-include-tests
  SRCS
    fpclassifyf_test.cpp
  DEPENDS
    libc.include.llvm-libc-macros.math_function_macros
)

add_libc_test(
  fpclassifyl_test
  SUITE
    libc-include-tests
  SRCS
    fpclassifyl_test.cpp
  DEPENDS
    libc.include.llvm-libc-macros.math_function_macros
)

add_libc_test(
  iszero_test
  SUITE
    libc-include-tests
  SRCS
    iszero_test.cpp
  DEPENDS
    libc.include.llvm-libc-macros.math_function_macros
)

add_libc_test(
  iszerof_test
  SUITE
    libc-include-tests
  SRCS
    iszerof_test.cpp
  DEPENDS
    libc.include.llvm-libc-macros.math_function_macros
)

add_libc_test(
  iszerol_test
  SUITE
    libc-include-tests
  SRCS
    iszerol_test.cpp
  DEPENDS
    libc.include.llvm-libc-macros.math_function_macros
)

add_libc_test(
  netinet_in_test
  SUITE
    libc-include-tests
  SRCS
    netinet_in_test.cpp
  DEPENDS
    libc.include.llvm-libc-macros.netinet_in_macros
)

add_subdirectory(net)

add_libc_test(
  signbit_test
  SUITE
    libc-include-tests
  SRCS
    signbit_test.cpp
  DEPENDS
    libc.include.llvm-libc-macros.math_function_macros
)

add_libc_test(
  signbitf_test
  SUITE
    libc-include-tests
  SRCS
    signbitf_test.cpp
  DEPENDS
    libc.include.llvm-libc-macros.math_function_macros
)

add_libc_test(
  signbitl_test
  SUITE
    libc-include-tests
  SRCS
    signbitl_test.cpp
  DEPENDS
    libc.include.llvm-libc-macros.math_function_macros
)

add_libc_test(
  isnan_test
  SUITE
    libc-include-tests
  SRCS
    isnan_test.cpp
  DEPENDS
    libc.include.llvm-libc-macros.math_function_macros
)

add_libc_test(
  isnanf_test
  SUITE
    libc-include-tests
  SRCS
    isnanf_test.cpp
  DEPENDS
    libc.include.llvm-libc-macros.math_function_macros
)

add_libc_test(
  isnanl_test
  SUITE
    libc-include-tests
  SRCS
    isnanl_test.cpp
  DEPENDS
    libc.include.llvm-libc-macros.math_function_macros
)

add_libc_test(
  isinf_test
  SUITE
    libc-include-tests
  SRCS
    isinf_test.cpp
  DEPENDS
    libc.include.llvm-libc-macros.math_function_macros
)

add_libc_test(
  isinff_test
  SUITE
    libc-include-tests
  SRCS
    isinff_test.cpp
  DEPENDS
    libc.include.llvm-libc-macros.math_function_macros
)

add_libc_test(
  isinfl_test
  SUITE
    libc-include-tests
  SRCS
    isinfl_test.cpp
  DEPENDS
    libc.include.llvm-libc-macros.math_function_macros
)

add_libc_test(
  isfinite_test
  SUITE
    libc-include-tests
  SRCS
    isfinite_test.cpp
  DEPENDS
    libc.include.llvm-libc-macros.math_function_macros
)

add_libc_test(
  isfinitef_test
  SUITE
    libc-include-tests
  SRCS
    isfinitef_test.cpp
  DEPENDS
    libc.include.llvm-libc-macros.math_function_macros
)

add_libc_test(
  isfinitel_test
  SUITE
    libc-include-tests
  SRCS
    isfinitel_test.cpp
  DEPENDS
    libc.include.llvm-libc-macros.math_function_macros
)

add_libc_test(
  signbit_c_test
  C_TEST
  SUITE
    libc-include-tests
  SRCS
    signbit_test.c
  COMPILE_OPTIONS
    -Wall
    -Werror
  DEPENDS
    libc.include.llvm-libc-macros.math_function_macros
)

add_libc_test(
  isnan_c_test
  C_TEST
  SUITE
    libc-include-tests
  SRCS
    isnan_test.c
  COMPILE_OPTIONS
    -Wall
    -Werror
  DEPENDS
    libc.include.llvm-libc-macros.math_function_macros
)

add_libc_test(
  isnormal_c_test
  C_TEST
  SUITE
    libc-include-tests
  SRCS
    isnormal_test.c
  COMPILE_OPTIONS
    -Wall
    -Werror
  DEPENDS
    libc.include.llvm-libc-macros.math_function_macros
)

# TODO(https://github.com/llvm/llvm-project/issues/114618): fix linkage failures
# add_libc_test(
#   issignaling_c_test
#   C_TEST
#   SUITE
#     libc-include-tests
#   SRCS
#     issignaling_test.c
#   COMPILE_OPTIONS
#     -Wall
#     -Werror
#   DEPENDS
#     libc.include.llvm-libc-macros.math_function_macros
#     libc.src.math.issignaling
#     libc.src.math.issignalingf
#     libc.src.math.issignalingl
# )

# add_libc_test(
#   iscanonical_c_test
#   C_TEST
#   SUITE
#     libc-include-tests
#   SRCS
#     iscanonical_test.c
#   COMPILE_OPTIONS
#     -Wall
#     -Werror
#   DEPENDS
#     libc.include.llvm-libc-macros.math_function_macros
#     libc.src.math.iscanonical
#     libc.src.math.iscanonicalf
#     libc.src.math.iscanonicall
#     libc.src.math.iscanonicalf16
#     libc.src.math.iscanonicalf128
# )

add_libc_test(
  isinf_c_test
  C_TEST
  SUITE
    libc-include-tests
  SRCS
    isinf_test.c
  COMPILE_OPTIONS
    -Wall
    -Werror
  DEPENDS
    libc.include.llvm-libc-macros.math_function_macros
)

add_libc_test(
  isfinite_c_test
  C_TEST
  SUITE
    libc-include-tests
  SRCS
    isfinite_test.c
  COMPILE_OPTIONS
    -Wall
    -Werror
  DEPENDS
    libc.include.llvm-libc-macros.math_function_macros
)

add_libc_test(
  issubnormal_c_test
  C_TEST
  SUITE
    libc-include-tests
  SRCS
    issubnormal_test.c
  COMPILE_OPTIONS
    -Wall
    -Werror
  DEPENDS
    libc.include.llvm-libc-macros.math_function_macros
)

add_libc_test(
  fpclassify_c_test
  C_TEST
  SUITE
    libc-include-tests
  SRCS
    fpclassify_test.c
  COMPILE_OPTIONS
    -Wall
    -Werror
  DEPENDS
    libc.include.llvm-libc-macros.math_function_macros
)

add_libc_test(
  iszero_c_test
  C_TEST
  SUITE
    libc-include-tests
  SRCS
    iszero_test.c
  COMPILE_OPTIONS
    -Wall
    -Werror
  DEPENDS
    libc.include.llvm-libc-macros.math_function_macros
)

add_libc_test(
  math_constants_c_test
  C_TEST
  SUITE
    libc-include-tests
  SRCS
    math_constants_test.c
  COMPILE_OPTIONS
    -Wall
    -Werror
  DEPENDS
    libc.include.llvm-libc-macros.math_macros
)

add_libc_test(
  elf_macros_test
  SUITE
    libc-include-tests
  SRCS
    elf_macros_test.cpp
  DEPENDS
    libc.include.llvm-libc-macros.elf_macros
)

add_libc_test(
  linux_signal_macros_test
  SUITE
    libc-include-tests
  SRCS
    linux_signal_macros_test.cpp
  DEPENDS
    libc.include.llvm-libc-macros.signal_macros
)

add_libc_test(
  stdio_macros_test
  SUITE
    libc-include-tests
  SRCS
    stdio_macros_test.cpp
  DEPENDS
    libc.include.llvm-libc-macros.stdio_macros
)

add_libc_test(
  limits_macros_test
  SUITE
    libc-include-tests
  SRCS
    limits_macros_test.cpp
  DEPENDS
    libc.include.llvm-libc-macros.limits_macros
)

add_libc_test(
  linux_unistd_macros_test
  SUITE
    libc-include-tests
  SRCS
    linux_unistd_macros_test.cpp
  DEPENDS
    libc.include.llvm-libc-macros.unistd_macros
)

add_libc_test(
  fnmatch_macros_test
  SUITE
    libc-include-tests
  SRCS
    fnmatch_macros_test.cpp
  DEPENDS
    libc.include.llvm-libc-macros.fnmatch_macros
)

add_libc_test(
  spawn_macros_test
  SUITE
    libc-include-tests
  SRCS
    spawn_macros_test.cpp
  DEPENDS
    libc.include.llvm-libc-macros.spawn_macros
)

add_libc_test(
  linux_sys_resource_macros_test
  SUITE
    libc-include-tests
  SRCS
    linux_sys_resource_macros_test.cpp
  DEPENDS
    libc.include.llvm-libc-macros.sys_resource_macros
)

add_libc_test(
  linux_termios_macros_test
  SUITE
    libc-include-tests
  SRCS
    linux_termios_macros_test.cpp
  DEPENDS
    libc.include.llvm-libc-macros.termios_macros
)

add_libc_test(
  linux_netdb_macros_test
  SUITE
    libc-include-tests
  SRCS
    linux_netdb_macros_test.cpp
  DEPENDS
    libc.include.llvm-libc-macros.netdb_macros
)

add_libc_test(
  linux_sys_socket_macros_test
  SUITE
    libc-include-tests
  SRCS
    linux_sys_socket_macros_test.cpp
  DEPENDS
    libc.include.llvm-libc-macros.sys_socket_macros
)

add_libc_test(
  netinet_in_macros_test
  SUITE
    libc-include-tests
  SRCS
    netinet_in_macros_test.cpp
  DEPENDS
    libc.include.llvm-libc-macros.netinet_in_macros
)

add_libc_test(
  linux_fcntl_macros_test
  SUITE
    libc-include-tests
  SRCS
    linux_fcntl_macros_test.cpp
  DEPENDS
    libc.include.llvm-libc-macros.fcntl_macros
)

add_libc_test(
  linux_sys_mount_macros_test
  SUITE
    libc-include-tests
  SRCS
    linux_sys_mount_macros_test.cpp
  DEPENDS
    libc.include.llvm-libc-macros.sys_mount_macros
)

add_libc_test(
  linux_sys_stat_macros_test
  SUITE
    libc-include-tests
  SRCS
    linux_sys_stat_macros_test.cpp
  DEPENDS
    libc.include.llvm-libc-macros.sys_stat_macros
)

add_libc_test(
  linux_sys_statvfs_macros_test
  SUITE
    libc-include-tests
  SRCS
    linux_sys_statvfs_macros_test.cpp
  DEPENDS
    libc.include.llvm-libc-macros.sys_statvfs_macros
)

add_libc_test(
  loff_t_test
  SUITE
    libc-include-tests
  SRCS
    loff_t_test.cpp
  DEPENDS
    libc.include.llvm-libc-types.loff_t
    libc.include.llvm-libc-types.off_t
)

add_libc_test(
  linux_sched_macros_test
  SUITE
    libc-include-tests
  SRCS
    linux_sched_macros_test.cpp
  DEPENDS
    libc.include.llvm-libc-macros.sched_macros
)

add_libc_test(
  linux_sys_sem_macros_test
  SUITE
    libc-include-tests
  SRCS
    linux_sys_sem_macros_test.cpp
  DEPENDS
    libc.include.llvm-libc-macros.sys_sem_macros
)

add_libc_test(
  linux_time_macros_test
  SUITE
    libc-include-tests
  SRCS
    linux_time_macros_test.cpp
  DEPENDS
    libc.include.llvm-libc-macros.time_macros
)

add_libc_test(
  sys_mman_macros_test
  SUITE
    libc-include-tests
  SRCS
    sys_mman_macros_test.cpp
  DEPENDS
    libc.include.llvm-libc-macros.sys_mman_macros
)

add_libc_test(
  mallinfo_test
  SUITE
    libc-include-tests
  SRCS
    mallinfo_test.cpp
  DEPENDS
    libc.include.llvm-libc-types.struct_mallinfo
    libc.include.llvm-libc-types.struct_mallinfo2
)

add_libc_test(
  pthread_macros_test
  SUITE
    libc-include-tests
  SRCS
    pthread_macros_test.cpp
  DEPENDS
    libc.include.llvm-libc-macros.pthread_macros
)

add_libc_test(
  barrier_type_bool_test
  SUITE
    libc-include-tests
  SRCS
    barrier_type_bool_test.cpp
  DEPENDS
    libc.include.llvm-libc-types.__barrier_type
    libc.include.llvm-libc-types.pthread_barrierattr_t
)

add_libc_test(
  char16_char32_test
  SUITE
    libc-include-tests
  SRCS
    char16_char32_test.cpp
  DEPENDS
    libc.include.llvm-libc-types.char16_t
    libc.include.llvm-libc-types.char32_t
)

add_libc_test(
  socklen_t_test
  SUITE
    libc-include-tests
  SRCS
    socklen_t_test.cpp
  DEPENDS
    libc.include.llvm-libc-types.socklen_t
)

add_libc_test(
  struct_epoll_event_test
  SUITE
    libc-include-tests
  SRCS
    struct_epoll_event_test.cpp
  DEPENDS
    libc.include.llvm-libc-types.struct_epoll_event
)

add_libc_test(
  dirent_macros_test
  SUITE
    libc-include-tests
  SRCS
    dirent_macros_test.cpp
  DEPENDS
    libc.include.llvm-libc-macros.dirent_macros
)

add_libc_test(
  linux_dirent_macros_test
  SUITE
    libc-include-tests
  SRCS
    linux_dirent_macros_test.cpp
  DEPENDS
    libc.include.llvm-libc-macros.dirent_macros
)

add_libc_test(
  struct_f_owner_ex_test
  SUITE
    libc-include-tests
  SRCS
    struct_f_owner_ex_test.cpp
  DEPENDS
    libc.include.llvm-libc-types.struct_f_owner_ex
)

add_libc_test(
  sys_uio_macros_test
  SUITE
    libc-include-tests
  SRCS
    sys_uio_macros_test.cpp
  DEPENDS
    libc.include.llvm-libc-macros.sys_uio_macros
)

add_libc_test(
  linux_sys_ioctl_macros_test
  SUITE
    libc-include-tests
  SRCS
    linux_sys_ioctl_macros_test.cpp
  DEPENDS
    libc.include.llvm-libc-macros.sys_ioctl_macros
)

add_libc_test(
  linux_sys_wait_macros_test
  SUITE
    libc-include-tests
  SRCS
    linux_sys_wait_macros_test.cpp
  DEPENDS
    libc.include.llvm-libc-macros.sys_wait_macros
)

add_libc_test(
  malloc_macros_test
  SUITE
    libc-include-tests
  SRCS
    malloc_macros_test.cpp
  DEPENDS
    libc.include.llvm-libc-macros.malloc_macros
)

# Test `#include <...>` of each header in each available language mode.
# This is gated on -DLLVM_LIBC_BUILD_HEADER_TESTS=ON until all the bugs
# in headers are fixed so the tests all compile.
set(TEST_STDC_VERSIONS 89;99;11;17;23)
set(TEST_STDCXX_VERSIONS 03;11;14;17;20;23;26)

function(add_header_test target_name source_file deps std_mode)
  if(LLVM_LIBC_BUILD_HEADER_TESTS)
    add_libc_test(
      ${target_name}
      C_TEST
      FULL_BUILD_ONLY
      SUITE
        libc-include-tests
      SRCS
        ${source_file}
      COMPILE_OPTIONS
        -Werror
        -Wsystem-headers
        -Wall
        -Wextra
        -std=${std_mode}
      DEPENDS
        ${deps}
    )
  endif()
endfunction()

foreach(target ${TARGET_PUBLIC_HEADERS})
  string(REPLACE "libc.include." "" header ${target})
  get_target_property(HEADER_NAME ${target} HEADER_NAME)

  set(test_stdc_file "${CMAKE_CURRENT_BINARY_DIR}/${header}_test.c")
  configure_file(header-test-template.c ${test_stdc_file} @ONLY)
  foreach(stdc_version ${TEST_STDC_VERSIONS})
    add_header_test(
      "${header}_c${stdc_version}_test"
      ${test_stdc_file}
      ${target}
      "c${stdc_version}"
    )
    add_header_test(
      "${header}_gnu${stdc_version}_test"
      ${test_stdc_file}
      ${target}
      "gnu${stdc_version}"
    )
  endforeach()

  set(test_stdcxx_file "${CMAKE_CURRENT_BINARY_DIR}/${header}_test.cpp")
  configure_file(header-test-template.c ${test_stdcxx_file} @ONLY)
  foreach(stdcxx_version ${TEST_STDCXX_VERSIONS})
    add_header_test(
      "${header}_cpp${stdcxx_version}_test"
      ${test_stdcxx_file}
      ${target}
      "c++${stdcxx_version}"
    )
    add_header_test(
      "${header}_gnucpp${stdcxx_version}_test"
      ${test_stdcxx_file}
      ${target}
      "gnu++${stdcxx_version}"
    )
  endforeach()
endforeach()

add_libc_test(
  netinet_icmp6_macros_test
  SUITE
    libc-include-tests
  SRCS
    netinet_icmp6_macros_test.cpp
  DEPENDS
    libc.include.llvm-libc-macros.netinet_icmp6_macros
    libc.include.llvm-libc-types.struct_icmp6_hdr
)
