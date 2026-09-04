//===-- Macros defined in sys/fanotify.h header file ----------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_MACROS_LINUX_SYS_FANOTIFY_MACROS_H
#define LLVM_LIBC_MACROS_LINUX_SYS_FANOTIFY_MACROS_H

// What happened, which is both what a mark asks about and what an event
// reports.
#define FAN_ACCESS 0x00000001        // The file was read.
#define FAN_MODIFY 0x00000002        // It was written.
#define FAN_ATTRIB 0x00000004        // Its metadata changed.
#define FAN_CLOSE_WRITE 0x00000008   // One opened for writing was closed.
#define FAN_CLOSE_NOWRITE 0x00000010 // One opened for reading was closed.
#define FAN_OPEN 0x00000020          // It was opened.
#define FAN_MOVED_FROM 0x00000040    // It was moved out of the directory.
#define FAN_MOVED_TO 0x00000080      // It was moved into it.
#define FAN_CREATE 0x00000100        // Something was made in the directory.
#define FAN_DELETE 0x00000200        // Something was removed from it.
#define FAN_DELETE_SELF 0x00000400   // The watched thing itself went.
#define FAN_MOVE_SELF 0x00000800     // It was moved.
#define FAN_OPEN_EXEC 0x00001000     // It was opened to be run.
#define FAN_RENAME 0x10000000        // It was renamed.

// Mount events, reported to a group opened with FAN_REPORT_MNT.
#define FAN_MNT_ATTACH 0x01000000
#define FAN_MNT_DETACH 0x02000000

// What the kernel says of its own accord.
#define FAN_Q_OVERFLOW 0x00004000 // Events were lost.
#define FAN_FS_ERROR 0x00008000   // The filesystem reported an error.

// Events which wait for the reading program to allow or deny them.
#define FAN_OPEN_PERM 0x00010000
#define FAN_ACCESS_PERM 0x00020000
#define FAN_OPEN_EXEC_PERM 0x00040000
#define FAN_PRE_ACCESS 0x00100000

// Qualifiers, which go alongside the events above.
#define FAN_EVENT_ON_CHILD 0x08000000 // Watch the directory's children too.
#define FAN_ONDIR 0x40000000          // Report events on directories.

// The pairs which are usually wanted together.
#define FAN_CLOSE (FAN_CLOSE_WRITE | FAN_CLOSE_NOWRITE)
#define FAN_MOVE (FAN_MOVED_FROM | FAN_MOVED_TO)

// Flags to fanotify_init. The class says how much say the group has over
// the events it is told about; only one of the three may be given.
#define FAN_CLOEXEC 0x00000001
#define FAN_NONBLOCK 0x00000002
#define FAN_CLASS_NOTIF 0x00000000
#define FAN_CLASS_CONTENT 0x00000004
#define FAN_CLASS_PRE_CONTENT 0x00000008
#define FAN_UNLIMITED_QUEUE 0x00000010
#define FAN_UNLIMITED_MARKS 0x00000020
#define FAN_ENABLE_AUDIT 0x00000040

// What an event should carry with it.
#define FAN_REPORT_PIDFD 0x00000080
#define FAN_REPORT_TID 0x00000100
#define FAN_REPORT_FID 0x00000200
#define FAN_REPORT_DIR_FID 0x00000400
#define FAN_REPORT_NAME 0x00000800
#define FAN_REPORT_TARGET_FID 0x00001000
#define FAN_REPORT_FD_ERROR 0x00002000
#define FAN_REPORT_MNT 0x00004000
#define FAN_REPORT_DFID_NAME (FAN_REPORT_DIR_FID | FAN_REPORT_NAME)
#define FAN_REPORT_DFID_NAME_TARGET                                            \
  (FAN_REPORT_DFID_NAME | FAN_REPORT_FID | FAN_REPORT_TARGET_FID)

// Flags to fanotify_mark: what to do, and what to do it to.
#define FAN_MARK_ADD 0x00000001
#define FAN_MARK_REMOVE 0x00000002
#define FAN_MARK_DONT_FOLLOW 0x00000004
#define FAN_MARK_ONLYDIR 0x00000008
#define FAN_MARK_IGNORED_MASK 0x00000020
#define FAN_MARK_IGNORED_SURV_MODIFY 0x00000040
#define FAN_MARK_FLUSH 0x00000080
#define FAN_MARK_EVICTABLE 0x00000200
#define FAN_MARK_IGNORE 0x00000400
#define FAN_MARK_INODE 0x00000000
#define FAN_MARK_MOUNT 0x00000010
#define FAN_MARK_FILESYSTEM 0x00000100
#define FAN_MARK_MNTNS 0x00000110
#define FAN_MARK_IGNORE_SURV (FAN_MARK_IGNORE | FAN_MARK_IGNORED_SURV_MODIFY)

// The version an event carries, so a program can tell it understands the
// layout it was handed.
#define FANOTIFY_METADATA_VERSION 3

// Which of the info records follows an event.
#define FAN_EVENT_INFO_TYPE_FID 1
#define FAN_EVENT_INFO_TYPE_DFID_NAME 2
#define FAN_EVENT_INFO_TYPE_DFID 3
#define FAN_EVENT_INFO_TYPE_PIDFD 4
#define FAN_EVENT_INFO_TYPE_ERROR 5
#define FAN_EVENT_INFO_TYPE_RANGE 6
#define FAN_EVENT_INFO_TYPE_MNT 7
#define FAN_EVENT_INFO_TYPE_OLD_DFID_NAME 10
#define FAN_EVENT_INFO_TYPE_NEW_DFID_NAME 12

// What a response may carry with it.
#define FAN_RESPONSE_INFO_NONE 0
#define FAN_RESPONSE_INFO_AUDIT_RULE 1

// The answer to a permission event.
#define FAN_ALLOW 0x01
#define FAN_DENY 0x02
#define FAN_AUDIT 0x10 // Also record the decision in the audit log.
#define FAN_INFO 0x20  // A response info record follows.

// A denial may name the error the open should fail with. It goes in the
// top bits of the response.
#define FAN_ERRNO_BITS 8
#define FAN_ERRNO_SHIFT (32 - FAN_ERRNO_BITS)
#define FAN_ERRNO_MASK ((1 << FAN_ERRNO_BITS) - 1)
#define FAN_DENY_ERRNO(err)                                                    \
  (FAN_DENY | ((((unsigned int)(err)) & FAN_ERRNO_MASK) << FAN_ERRNO_SHIFT))

// What the fd of an event is when there is none to give.
#define FAN_NOFD -1
#define FAN_NOPIDFD FAN_NOFD
#define FAN_EPIDFD -2

// Walking a block of events read from the group. len is the amount left,
// and both macros take it down as they go.
#define FAN_EVENT_METADATA_LEN (sizeof(struct fanotify_event_metadata))
#define FAN_EVENT_NEXT(meta, len)                                              \
  ((len) -= (meta)->event_len,                                                 \
   (struct fanotify_event_metadata *)(((char *)(meta)) + (meta)->event_len))
#define FAN_EVENT_OK(meta, len)                                                \
  ((long)(len) >= (long)FAN_EVENT_METADATA_LEN &&                              \
   (long)(meta)->event_len >= (long)FAN_EVENT_METADATA_LEN &&                  \
   (long)(meta)->event_len <= (long)(len))

#endif // LLVM_LIBC_MACROS_LINUX_SYS_FANOTIFY_MACROS_H
