//===-- Macros defined in scsi/scsi_ioctl.h header file -------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_MACROS_SCSI_SCSI_IOCTL_MACROS_H
#define LLVM_LIBC_MACROS_SCSI_SCSI_IOCTL_MACROS_H

// Requests to the SCSI layer. The low numbers are those the older ioctl
// interface took a command through.
#define SCSI_IOCTL_SEND_COMMAND 1
#define SCSI_IOCTL_TEST_UNIT_READY 2
#define SCSI_IOCTL_BENCHMARK_COMMAND 3
#define SCSI_IOCTL_SYNC 4
#define SCSI_IOCTL_START_UNIT 5
#define SCSI_IOCTL_STOP_UNIT 6
#define SCSI_IOCTL_DOORLOCK 0x5380
#define SCSI_IOCTL_DOORUNLOCK 0x5381

#endif // LLVM_LIBC_MACROS_SCSI_SCSI_IOCTL_MACROS_H
