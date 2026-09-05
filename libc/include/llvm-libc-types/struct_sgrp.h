//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of struct sgrp.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_SGRP_H
#define LLVM_LIBC_TYPES_STRUCT_SGRP_H

// One entry of the shadow group database. Both lists end with a null
// pointer.
struct sgrp {
  char *sg_namp;   // The group name.
  char *sg_passwd; // The hashed password.
  char **sg_adm;   // Who may administer the group.
  char **sg_mem;   // Who is in it.
};

#endif // LLVM_LIBC_TYPES_STRUCT_SGRP_H
