//===-- Definition of pthread macros --------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_MACROS_PTHREAD_MACRO_H
#define LLVM_LIBC_MACROS_PTHREAD_MACRO_H

#define PTHREAD_NULL {0}

#define PTHREAD_CREATE_JOINABLE 0
#define PTHREAD_CREATE_DETACHED 1

// Whether a thread may be cancelled at all.
#define PTHREAD_CANCEL_ENABLE 0
#define PTHREAD_CANCEL_DISABLE 1

// When a thread acts on a cancellation request: at its next cancellation
// point, or as soon as the request arrives.
#define PTHREAD_CANCEL_DEFERRED 0
#define PTHREAD_CANCEL_ASYNCHRONOUS 1

// What joining a cancelled thread gives back.
#define PTHREAD_CANCELED ((void *)-1)

// A cleanup handler lives in the frame that pushed it, which is why these two
// have to appear in the same block as each other: the opening brace is in one
// and the closing brace in the other.
#define pthread_cleanup_push(routine, arg)                                     \
  {                                                                            \
    struct _pthread_cleanup_buffer __llvm_libc_cleanup_buffer;                 \
    _pthread_cleanup_push(&__llvm_libc_cleanup_buffer, (routine), (arg));

#define pthread_cleanup_pop(execute)                                           \
  _pthread_cleanup_pop(&__llvm_libc_cleanup_buffer, (execute));                \
  }

#define PTHREAD_MUTEX_NORMAL 0
#define PTHREAD_MUTEX_ERRORCHECK 1
#define PTHREAD_MUTEX_RECURSIVE 2
#define PTHREAD_MUTEX_DEFAULT PTHREAD_MUTEX_NORMAL

#define PTHREAD_MUTEX_STALLED 0
#define PTHREAD_MUTEX_ROBUST 1

#define PTHREAD_BARRIER_SERIAL_THREAD -1

#define PTHREAD_ONCE_INIT {0}

#define PTHREAD_PROCESS_PRIVATE 0
#define PTHREAD_PROCESS_SHARED 1

#ifdef __linux__
#define PTHREAD_MUTEX_INITIALIZER                                              \
  {                                                                            \
      /* .__ftxw = */ {0},    /* .__priority_inherit = */ 0,                   \
      /* .__recursive = */ 0, /* .__robust = */ 0,                             \
      /* .__pshared = */ 0,   /* .__error_checking = */ 0,                     \
      /* .__owner = */ 0,     /* .__lock_count = */ 0,                         \
  }
#else
#define PTHREAD_MUTEX_INITIALIZER                                              \
  {                                                                            \
      /* .__ftxw = */ {0},    /* .__priority_inherit = */ 0,                   \
      /* .__recursive = */ 0, /* .__robust = */ 0,                             \
      /* .__pshared = */ 0,   /* .__error_checking = */ 0,                     \
      /* .__owner = */ 0,     /* .__lock_count = */ 0,                         \
  }
#endif

#define PTHREAD_COND_INITIALIZER                                               \
  {                                                                            \
      /* .__waiter_queue = */ {{NULL, NULL}},                                  \
      /* .__futex = */ {0},                                                    \
      /* .__is_shared = */ 0,                                                  \
      /* .__is_realtime = */ 1,                                                \
      /* .__padding = */ {0},                                                  \
  }

#define PTHREAD_RWLOCK_INITIALIZER                                             \
  {                                                                            \
      /* .__raw = */ {                                                         \
          /* .__is_pshared = */ 0,                                             \
          /* .__preference = */ 0,                                             \
          /* .__state = */ 0,                                                  \
          /* .__wait_queue_mutex = */ {0},                                     \
          /* .__pending_readers = */ {0},                                      \
          /* .__pending_writers = */ {0},                                      \
          /* .__reader_serialization = */ {0},                                 \
          /* .__writer_serialization = */ {0},                                 \
      },                                                                       \
      /* .__write_tid = */ 0,                                                  \
  }

// glibc extensions
#define PTHREAD_STACK_MIN (1 << 14) // 16KB
#define PTHREAD_RWLOCK_PREFER_READER_NP 0
#define PTHREAD_RWLOCK_PREFER_WRITER_NP 1
#define PTHREAD_RWLOCK_PREFER_WRITER_NONRECURSIVE_NP 2

// llvm libc extensions
#define PTHREAD_STACK_DYNAMIC_NP 0

#endif // LLVM_LIBC_MACROS_PTHREAD_MACRO_H
