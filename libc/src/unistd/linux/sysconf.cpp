//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Linux implementation of sysconf
///
//===----------------------------------------------------------------------===//

#include "src/unistd/sysconf.h"

#include "src/__support/common.h"

#include "hdr/limits_macros.h"
#include "hdr/signal_macros.h"
#include "hdr/sys_auxv_macros.h"
#include "hdr/sys_resource_macros.h"
#include "hdr/types/struct_rlimit.h"
#include "hdr/types/struct_sysinfo.h"
#include "hdr/unistd_macros.h"
#include "src/__support/OSUtil/linux/auxv.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/prlimit.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/sysinfo.h"
#include "src/__support/OSUtil/linux/sysinfo.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include <linux/limits.h>

namespace LIBC_NAMESPACE_DECL {

namespace { // Anonymous namespace for internal helpers

// Fallback value for ARG_MAX when RLIMIT_STACK is RLIM_INFINITY (unlimited).
// When the stack is unlimited, the kernel caps the argument limit at 3/4 of the
// default stack limit (_STK_LIM = 8MB), which yields 6MB.
constexpr long DEFAULT_STACK_LIMIT = 8 * 1024 * 1024;          // 8MB
constexpr long ARG_MAX_FALLBACK = DEFAULT_STACK_LIMIT / 4 * 3; // 6MB

long get_arg_max() {
  struct rlimit limits;
  ErrorOr<int> ret = linux_syscalls::getrlimit(RLIMIT_STACK, &limits);
  if (!ret) {
    libc_errno = -ret.error();
    return -1;
  }
  if (limits.rlim_cur == ~0ULL)
    return ARG_MAX_FALLBACK;

  long val = static_cast<long>(limits.rlim_cur / 4);
  return val > ARG_MAX ? val : ARG_MAX;
}

long get_open_max() {
  struct rlimit limits;
  ErrorOr<int> ret = linux_syscalls::getrlimit(RLIMIT_NOFILE, &limits);
  if (!ret) {
    libc_errno = -ret.error();
    return -1;
  }
  if (limits.rlim_cur == ~0ULL)
    return -1;
  return static_cast<long>(limits.rlim_cur);
}

long get_page_size() {
  cpp::optional<unsigned long> page_size = auxv::get(AT_PAGESZ);
  if (page_size)
    return static_cast<long>(*page_size);
  libc_errno = EINVAL;
  return -1;
}

long get_nprocessors_conf() {
  return static_cast<long>(
      sysinfo::parse_nproc_with_fallback_from(sysinfo::POSSIBLE_NPROC_PATH));
}

long get_nprocessors_onln() {
  return static_cast<long>(
      sysinfo::parse_nproc_with_fallback_from(sysinfo::ONLINE_NPROC_PATH));
}

long get_phys_pages() {
  struct ::sysinfo info;
  ErrorOr<int> ret = linux_syscalls::sysinfo(&info);
  if (!ret) {
    libc_errno = -ret.error();
    return -1;
  }
  cpp::optional<unsigned long> page_size = auxv::get(AT_PAGESZ);
  if (!page_size) {
    libc_errno = ENOSYS;
    return -1;
  }
  unsigned long ps = *page_size;
  unsigned long mem_unit = info.mem_unit;
  unsigned long num = info.totalram;
  if (mem_unit >= ps) {
    num *= (mem_unit / ps);
  } else {
    num /= (ps / mem_unit);
  }
  return static_cast<long>(num);
}

// The kernel keeps some of the limits as resource limits rather than as
// constants. No limit is spelled as no answer, which a caller reads as
// unbounded.
long get_rlimit(int resource) {
  struct rlimit limits;
  ErrorOr<int> ret = linux_syscalls::getrlimit(resource, &limits);
  if (!ret) {
    libc_errno = -ret.error();
    return -1;
  }
  if (limits.rlim_cur == ~0ULL)
    return -1;
  return static_cast<long>(limits.rlim_cur);
}

long get_child_max() { return get_rlimit(RLIMIT_NPROC); }

long get_sigqueue_max() { return get_rlimit(RLIMIT_SIGPENDING); }

long get_avphys_pages() {
  struct ::sysinfo info;
  ErrorOr<int> ret = linux_syscalls::sysinfo(&info);
  if (!ret) {
    libc_errno = -ret.error();
    return -1;
  }
  cpp::optional<unsigned long> page_size = auxv::get(AT_PAGESZ);
  if (!page_size) {
    libc_errno = ENOSYS;
    return -1;
  }
  unsigned long ps = *page_size;
  unsigned long mem_unit = info.mem_unit;
  unsigned long num = info.freeram;
  if (mem_unit >= ps) {
    num *= (mem_unit / ps);
  } else {
    num /= (ps / mem_unit);
  }
  return static_cast<long>(num);
}

} // anonymous namespace

LLVM_LIBC_FUNCTION(long, sysconf, (int name)) {
  switch (name) {
  case _SC_ARG_MAX:
    return get_arg_max();
  case _SC_CLK_TCK:
    // On Linux, this is a counterpart of kernel constant USER_HZ, which is
    // set to 100 on most platforms for user-space compatibility:
    // https://github.com/torvalds/linux/blob/master/include/uapi/asm-generic/param.h#L6
    return 100;
  case _SC_PAGESIZE:
    return get_page_size();
  case _SC_NPROCESSORS_CONF:
    return get_nprocessors_conf();
  case _SC_NPROCESSORS_ONLN:
    return get_nprocessors_onln();
  case _SC_VERSION:
    return _POSIX_VERSION;
  case _SC_THREADS:
    return _POSIX_THREADS;
  case _SC_THREAD_ATTR_STACKADDR:
    return _POSIX_THREAD_ATTR_STACKADDR;
  case _SC_THREAD_ATTR_STACKSIZE:
    return _POSIX_THREAD_ATTR_STACKSIZE;
  case _SC_THREAD_CPUTIME:
    return _POSIX_THREAD_CPUTIME;
  case _SC_THREAD_PRIO_INHERIT:
    return _POSIX_THREAD_PRIO_INHERIT;
  case _SC_THREAD_PRIO_PROTECT:
    return _POSIX_THREAD_PRIO_PROTECT;
  case _SC_THREAD_PRIORITY_SCHEDULING:
    return _POSIX_THREAD_PRIORITY_SCHEDULING;
  case _SC_THREAD_PROCESS_SHARED:
    return _POSIX_THREAD_PROCESS_SHARED;
  case _SC_THREAD_ROBUST_PRIO_INHERIT:
    return _POSIX_THREAD_ROBUST_PRIO_INHERIT;
  case _SC_THREAD_ROBUST_PRIO_PROTECT:
    return _POSIX_THREAD_ROBUST_PRIO_PROTECT;
  case _SC_THREAD_SAFE_FUNCTIONS:
    return _POSIX_THREAD_SAFE_FUNCTIONS;
  case _SC_THREAD_SPORADIC_SERVER:
    return _POSIX_THREAD_SPORADIC_SERVER;
  case _SC_THREAD_DESTRUCTOR_ITERATIONS:
    return PTHREAD_DESTRUCTOR_ITERATIONS;
  case _SC_GETGR_R_SIZE_MAX:
    // No recommended buffer size for getgrgid_r/getgrnam_r, as they work
    // with any user-supplied buffer.
    return -1;
  case _SC_GETPW_R_SIZE_MAX:
    // No recommended buffer size for getpwuid_r/getpwnam_r, as they work
    // with any user-supplied buffer.
    return -1;
  case _SC_OPEN_MAX:
    return get_open_max();
  case _SC_PHYS_PAGES:
    return get_phys_pages();

  // The limits which do not change while the system is running are the
  // <limits.h> values, and answering from there keeps the two agreeing.
  case _SC_CHILD_MAX:
    return get_child_max();
  case _SC_NGROUPS_MAX:
    return NGROUPS_MAX;
  case _SC_STREAM_MAX:
    return _POSIX_STREAM_MAX;
  case _SC_TZNAME_MAX:
    return _POSIX_TZNAME_MAX;
  case _SC_LINE_MAX:
    return _POSIX2_LINE_MAX;
  case _SC_RE_DUP_MAX:
    return RE_DUP_MAX;
  case _SC_IOV_MAX:
    return IOV_MAX;
  case _SC_HOST_NAME_MAX:
    return HOST_NAME_MAX;
  case _SC_LOGIN_NAME_MAX:
    return LOGIN_NAME_MAX;
  case _SC_TTY_NAME_MAX:
    return TTY_NAME_MAX;
  case _SC_SYMLOOP_MAX:
    return _POSIX_SYMLOOP_MAX;
  case _SC_THREAD_STACK_MIN:
    return PTHREAD_STACK_MIN;
  case _SC_THREAD_KEYS_MAX:
    return PTHREAD_KEYS_MAX;
  case _SC_THREAD_THREADS_MAX:
    return _POSIX_THREAD_THREADS_MAX;
  case _SC_ATEXIT_MAX:
    return _POSIX_ATEXIT_MAX;

  // The options which are supported, reported as the version of the edition
  // they come from, the same as the macros in <unistd.h> say.
  case _SC_JOB_CONTROL:
    return _POSIX_JOB_CONTROL;
  case _SC_SAVED_IDS:
    return _POSIX_SAVED_IDS;
  case _SC_TIMERS:
    return _POSIX_TIMERS;
  case _SC_MONOTONIC_CLOCK:
    return _POSIX_MONOTONIC_CLOCK;
  case _SC_CLOCK_SELECTION:
    return _POSIX_CLOCK_SELECTION;
  case _SC_SEMAPHORES:
    return _POSIX_SEMAPHORES;
  case _SC_TIMEOUTS:
    return _POSIX_TIMEOUTS;
  case _SC_REALTIME_SIGNALS:
    return _POSIX_REALTIME_SIGNALS;
  case _SC_PRIORITY_SCHEDULING:
    return _POSIX_PRIORITY_SCHEDULING;
  case _SC_MEMLOCK:
    return _POSIX_MEMLOCK;
  case _SC_MEMLOCK_RANGE:
    return _POSIX_MEMLOCK_RANGE;
  case _SC_MESSAGE_PASSING:
    return _POSIX_MESSAGE_PASSING;
  case _SC_SHARED_MEMORY_OBJECTS:
    return _POSIX_SHARED_MEMORY_OBJECTS;
  case _SC_ADVISORY_INFO:
    return _POSIX_ADVISORY_INFO;
  case _SC_CPUTIME:
    return _POSIX_CPUTIME;
  case _SC_READER_WRITER_LOCKS:
    return _POSIX_READER_WRITER_LOCKS;
  case _SC_SPIN_LOCKS:
    return _POSIX_SPIN_LOCKS;
  case _SC_REGEXP:
    return _POSIX_REGEXP;
  case _SC_SPAWN:
    return _POSIX_SPAWN;
  case _SC_IPV6:
    return _POSIX_IPV6;
  case _SC_RAW_SOCKETS:
    return _POSIX_RAW_SOCKETS;
  case _SC_BARRIERS:
    return _POSIX_BARRIERS;
  case _SC_2_C_BIND:
    return _POSIX2_C_BIND;
  case _SC_2_C_DEV:
    return _POSIX2_C_DEV;
  case _SC_2_CHAR_TERM:
    return _POSIX2_CHAR_TERM;
  case _SC_2_FORT_DEV:
    return _POSIX2_FORT_DEV;
  case _SC_2_FORT_RUN:
    return _POSIX2_FORT_RUN;
  case _SC_2_SW_DEV:
    return _POSIX2_SW_DEV;
  case _SC_2_LOCALEDEF:
    return _POSIX2_LOCALEDEF;
  case _SC_2_UPE:
    return _POSIX2_UPE;
  case _SC_2_PBS:
    return _POSIX2_PBS;
  case _SC_2_PBS_ACCOUNTING:
    return _POSIX2_PBS_ACCOUNTING;
  case _SC_2_PBS_LOCATE:
    return _POSIX2_PBS_LOCATE;
  case _SC_2_PBS_MESSAGE:
    return _POSIX2_PBS_MESSAGE;
  case _SC_2_PBS_TRACK:
    return _POSIX2_PBS_TRACK;
  case _SC_2_PBS_CHECKPOINT:
    return _POSIX2_PBS_CHECKPOINT;
  case _SC_ASYNCHRONOUS_IO:
    return _POSIX_ASYNCHRONOUS_IO;
  case _SC_SPORADIC_SERVER:
    return _POSIX_SPORADIC_SERVER;
  case _SC_TRACE:
    return _POSIX_TRACE;
  case _SC_TRACE_EVENT_FILTER:
    return _POSIX_TRACE_EVENT_FILTER;
  case _SC_TRACE_INHERIT:
    return _POSIX_TRACE_INHERIT;
  case _SC_TRACE_LOG:
    return _POSIX_TRACE_LOG;
  case _SC_TYPED_MEMORY_OBJECTS:
    return _POSIX_TYPED_MEMORY_OBJECTS;

  // The programming environments. Every target here has 64 bit file offsets,
  // and the pointer and long sizes decide the rest.
  case _SC_V7_ILP32_OFF32:
    return -1;
  case _SC_V7_ILP32_OFFBIG:
    return sizeof(int) == 4 && sizeof(long) == 4 && sizeof(void *) == 4 ? 1
                                                                        : -1;
  case _SC_V7_LP64_OFF64:
    return sizeof(int) == 4 && sizeof(long) == 8 && sizeof(void *) == 8 ? 1
                                                                        : -1;
  case _SC_V7_LPBIG_OFFBIG:
    return sizeof(int) >= 4 && sizeof(long) >= 8 && sizeof(void *) >= 8 ? 1
                                                                        : -1;

  // Limits with no fixed value. -1 says there is none, and errno stays as it
  // was.
  case _SC_SS_REPL_MAX:
  case _SC_TRACE_EVENT_NAME_MAX:
  case _SC_TRACE_NAME_MAX:
  case _SC_TRACE_SYS_MAX:
  case _SC_TRACE_USER_EVENT_MAX:
    return -1;
  case _SC_MAPPED_FILES:
    return _POSIX_MAPPED_FILES;
  case _SC_MEMORY_PROTECTION:
    return _POSIX_MEMORY_PROTECTION;
  case _SC_FSYNC:
    return _POSIX_FSYNC;
  case _SC_SYNCHRONIZED_IO:
    return _POSIX_SYNCHRONIZED_IO;
  case _SC_SHELL:
    return _POSIX_SHELL;
  case _SC_2_VERSION:
    return _POSIX2_VERSION;
  case _SC_XOPEN_VERSION:
    return _XOPEN_VERSION;
  case _SC_XOPEN_UNIX:
    return _XOPEN_UNIX;
  case _SC_XOPEN_CRYPT:
    return _XOPEN_CRYPT;
  case _SC_XOPEN_ENH_I18N:
    return _XOPEN_ENH_I18N;
  case _SC_XOPEN_SHM:
    return _XOPEN_SHM;
  case _SC_XOPEN_LEGACY:
    return _XOPEN_LEGACY;
  case _SC_XOPEN_REALTIME:
    return _XOPEN_REALTIME;
  case _SC_XOPEN_REALTIME_THREADS:
    return _XOPEN_REALTIME_THREADS;
  case _SC_XOPEN_STREAMS:
    return _XOPEN_STREAMS;

  // Limits which are fixed, and whose values <limits.h> already states.
  case _SC_SEM_VALUE_MAX:
    return SEM_VALUE_MAX;
  case _SC_DELAYTIMER_MAX:
    return DELAYTIMER_MAX;
  case _SC_MQ_PRIO_MAX:
    return MQ_PRIO_MAX;

  // The limits the standard leaves to the system and this one does not bound.
  case _SC_AIO_LISTIO_MAX:
  case _SC_AIO_MAX:
  case _SC_AIO_PRIO_DELTA_MAX:
  case _SC_MQ_OPEN_MAX:
  case _SC_SEM_NSEMS_MAX:
  case _SC_TIMER_MAX:
    return -1;
  case _SC_RTSIG_MAX:
    return SIGRTMAX - SIGRTMIN + 1;
  // The values of _POSIX2_BC_BASE_MAX and its relatives, as glibc answers.
  case _SC_BC_BASE_MAX:
    return 99;
  case _SC_BC_DIM_MAX:
    return 2048;
  case _SC_BC_SCALE_MAX:
    return 99;
  case _SC_BC_STRING_MAX:
    return 1000;
  case _SC_COLL_WEIGHTS_MAX:
    return 255;
  case _SC_EXPR_NEST_MAX:
    return 32;
  case _SC_SIGQUEUE_MAX:
    return get_sigqueue_max();
  // The size of a cache is not something this reads; 0 is the answer for
  // "unknown" that glibc gives where it cannot ask the processor.
  case _SC_LEVEL1_ICACHE_SIZE:
  case _SC_LEVEL1_ICACHE_ASSOC:
  case _SC_LEVEL1_ICACHE_LINESIZE:
  case _SC_LEVEL1_DCACHE_SIZE:
  case _SC_LEVEL1_DCACHE_ASSOC:
  case _SC_LEVEL1_DCACHE_LINESIZE:
  case _SC_LEVEL2_CACHE_SIZE:
  case _SC_LEVEL2_CACHE_ASSOC:
  case _SC_LEVEL2_CACHE_LINESIZE:
  case _SC_LEVEL3_CACHE_SIZE:
  case _SC_LEVEL3_CACHE_ASSOC:
  case _SC_LEVEL3_CACHE_LINESIZE:
  case _SC_LEVEL4_CACHE_SIZE:
  case _SC_LEVEL4_CACHE_ASSOC:
  case _SC_LEVEL4_CACHE_LINESIZE:
    return 0;

  case _SC_AVPHYS_PAGES:
    return get_avphys_pages();

  default:
    // TODO: Complete the rest of the sysconf options.
    libc_errno = EINVAL;
    return -1;
  }
}

} // namespace LIBC_NAMESPACE_DECL
