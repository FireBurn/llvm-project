//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of macros from fts.h
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_MACROS_FTS_MACROS_H
#define LLVM_LIBC_MACROS_FTS_MACROS_H

#define FTS_ROOTPARENTLEVEL -1
#define FTS_ROOTLEVEL 0
#define FTS_MAXLEVEL 0x7fff

// What fts_info says an entry is.
#define FTS_D 1       // A directory, before its contents.
#define FTS_DC 2      // A directory that is its own ancestor.
#define FTS_DEFAULT 3 // Something that is none of the others.
#define FTS_DNR 4     // A directory that could not be read.
#define FTS_DOT 5     // Dot or dot-dot.
#define FTS_DP 6      // A directory, after its contents.
#define FTS_ERR 7     // An error; fts_errno says which.
#define FTS_F 8       // A regular file.
#define FTS_INIT 9    // Nothing has been read yet.
#define FTS_NS 10     // stat failed.
#define FTS_NSOK 11   // stat was not asked for.
#define FTS_SL 12     // A symbolic link.
#define FTS_SLNONE 13 // A symbolic link with no target.

#define FTS_DONTCHDIR 0x01
#define FTS_SYMFOLLOW 0x02

// What fts_set can ask for.
#define FTS_AGAIN 1
#define FTS_FOLLOW 2
#define FTS_NOINSTR 3
#define FTS_SKIP 4

// Options for fts_open.
#define FTS_COMFOLLOW 0x0001
#define FTS_LOGICAL 0x0002
#define FTS_NOCHDIR 0x0004
#define FTS_NOSTAT 0x0008
#define FTS_PHYSICAL 0x0010
#define FTS_SEEDOT 0x0020
#define FTS_XDEV 0x0040
#define FTS_OPTIONMASK 0x00ff

// Option for fts_children.
#define FTS_NAMEONLY 0x1000

#endif // LLVM_LIBC_MACROS_FTS_MACROS_H
