//===-- Macros defined in sys/inotify.h header file -----------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_MACROS_LINUX_SYS_INOTIFY_MACROS_H
#define LLVM_LIBC_MACROS_LINUX_SYS_INOTIFY_MACROS_H

#include "fcntl-macros.h"

// What happened, which is what a watch asks about and what an event
// reports.
#define IN_ACCESS 0x00000001        // The file was read.
#define IN_MODIFY 0x00000002        // It was written.
#define IN_ATTRIB 0x00000004        // Its attributes changed.
#define IN_CLOSE_WRITE 0x00000008   // One opened for writing was closed.
#define IN_CLOSE_NOWRITE 0x00000010 // One opened for reading was closed.
#define IN_OPEN 0x00000020          // It was opened.
#define IN_MOVED_FROM 0x00000040    // It was moved out of the directory.
#define IN_MOVED_TO 0x00000080      // It was moved into it.
#define IN_CREATE 0x00000100        // It was made in the directory.
#define IN_DELETE 0x00000200        // It was removed from it.
#define IN_DELETE_SELF 0x00000400   // The watched thing itself went.
#define IN_MOVE_SELF 0x00000800     // It was moved.

// What the kernel says of its own accord.
#define IN_UNMOUNT 0x00002000    // The filesystem it was on went away.
#define IN_Q_OVERFLOW 0x00004000 // Events were lost.
#define IN_IGNORED 0x00008000    // The watch is gone.

// The two pairs which are usually wanted together.
#define IN_CLOSE (IN_CLOSE_WRITE | IN_CLOSE_NOWRITE)
#define IN_MOVE (IN_MOVED_FROM | IN_MOVED_TO)

// How to watch rather than what for.
#define IN_ONLYDIR 0x01000000     // Fail unless the path is a directory.
#define IN_DONT_FOLLOW 0x02000000 // Watch a link rather than its target.
#define IN_EXCL_UNLINK 0x04000000 // Stop reporting on an unlinked child.
#define IN_MASK_CREATE 0x10000000 // Fail if the path is watched already.
#define IN_MASK_ADD 0x20000000    // Add to the mask rather than replace it.
#define IN_ISDIR 0x40000000   // The thing the event is about is a directory.
#define IN_ONESHOT 0x80000000 // Report once, then drop the watch.

// Everything which can be asked for.
#define IN_ALL_EVENTS                                                          \
  (IN_ACCESS | IN_MODIFY | IN_ATTRIB | IN_CLOSE_WRITE | IN_CLOSE_NOWRITE |     \
   IN_OPEN | IN_MOVED_FROM | IN_MOVED_TO | IN_CREATE | IN_DELETE |             \
   IN_DELETE_SELF | IN_MOVE_SELF)

#define IN_CLOEXEC O_CLOEXEC
#define IN_NONBLOCK O_NONBLOCK

#endif // LLVM_LIBC_MACROS_LINUX_SYS_INOTIFY_MACROS_H
