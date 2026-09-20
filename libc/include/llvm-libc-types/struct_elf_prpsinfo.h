//===-- Definition of struct elf_prpsinfo ---------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_ELF_PRPSINFO_H
#define LLVM_LIBC_TYPES_STRUCT_ELF_PRPSINFO_H

#include "gid_t.h"
#include "pid_t.h"
#include "uid_t.h"

// How much of the command line the note keeps.
#define ELF_PRARGSZ 80

// The NT_PRPSINFO note of a core file: which process it was. The layout is
// the kernel's, so it is not to be rearranged.
struct elf_prpsinfo {
  char pr_state;
  char pr_sname;
  char pr_zomb;
  char pr_nice;
  unsigned long pr_flag;
  unsigned int pr_uid;
  unsigned int pr_gid;
  int pr_pid;
  int pr_ppid;
  int pr_pgrp;
  int pr_sid;
  char pr_fname[16];
  char pr_psargs[ELF_PRARGSZ];
};

#endif // LLVM_LIBC_TYPES_STRUCT_ELF_PRPSINFO_H
