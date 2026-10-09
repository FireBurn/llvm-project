#ifndef LLVM_LIBC_MACROS_UNISTD_MACROS_H
#define LLVM_LIBC_MACROS_UNISTD_MACROS_H

#ifdef __linux__
#include "linux/unistd-macros.h"
#endif

#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

// confstr() variables. _CS_PATH is the search path that finds the standard
// utilities, which is what a caller wants when it must not trust PATH.
#define _CS_PATH 0
// What glibc names its C library and thread library, which some programs ask
// for to report. The values are glibc's.
#define _CS_GNU_LIBC_VERSION 2
#define _CS_GNU_LIBPTHREAD_VERSION 3

// What lockf() is being asked to do to the region. These are the operation,
// not the kind of lock: every lock lockf takes is a write lock.
#define F_ULOCK 0
#define F_LOCK 1
#define F_TLOCK 2
#define F_TEST 3

#endif // LLVM_LIBC_MACROS_UNISTD_MACROS_H
