//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of struct sg_scsi_id.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_SG_SCSI_ID_H
#define LLVM_LIBC_TYPES_STRUCT_SG_SCSI_ID_H

// Where a device is, as SG_GET_SCSI_ID reports it.
struct sg_scsi_id {
  int host_no;
  int channel;
  int scsi_id;
  int lun;
  int scsi_type; // One of the TYPE_ values.
  short h_cmd_per_lun;
  short d_queue_depth;
  int unused[2];
};

typedef struct sg_scsi_id Sg_scsi_id;

#endif // LLVM_LIBC_TYPES_STRUCT_SG_SCSI_ID_H
