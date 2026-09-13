//===-- Macros defined in scsi/sg.h header file ---------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_MACROS_SCSI_SG_MACROS_H
#define LLVM_LIBC_MACROS_SCSI_SG_MACROS_H

// Which way the data of an sg_io_hdr moves.
#define SG_DXFER_NONE (-1)
#define SG_DXFER_TO_DEV (-2)
#define SG_DXFER_FROM_DEV (-3)
#define SG_DXFER_TO_FROM_DEV (-4)

// Bits in sg_io_hdr.flags.
#define SG_FLAG_DIRECT_IO 1
#define SG_FLAG_LUN_INHIBIT 2
#define SG_FLAG_NO_DXFER 0x10000

// Bits in sg_io_hdr.info.
#define SG_INFO_OK_MASK 0x1
#define SG_INFO_OK 0x0
#define SG_INFO_CHECK 0x1
#define SG_INFO_DIRECT_IO_MASK 0x6
#define SG_INFO_INDIRECT_IO 0x0
#define SG_INFO_DIRECT_IO 0x2
#define SG_INFO_MIXED_IO 0x4

// Requests a generic device node answers.
#define SG_EMULATED_HOST 0x2203
#define SG_SET_TRANSFORM 0x2204
#define SG_GET_TRANSFORM 0x2205
#define SG_SET_RESERVED_SIZE 0x2275
#define SG_GET_RESERVED_SIZE 0x2272
#define SG_GET_SCSI_ID 0x2276
#define SG_SET_FORCE_LOW_DMA 0x2279
#define SG_GET_LOW_DMA 0x227a
#define SG_SET_FORCE_PACK_ID 0x227b
#define SG_GET_PACK_ID 0x227c
#define SG_GET_NUM_WAITING 0x227d
#define SG_GET_SG_TABLESIZE 0x227f
#define SG_GET_VERSION_NUM 0x2282
#define SG_SCSI_RESET 0x2284
#define SG_IO 0x2285
#define SG_GET_REQUEST_TABLE 0x2286
#define SG_SET_KEEP_ORPHAN 0x2287
#define SG_GET_KEEP_ORPHAN 0x2288

// What SG_SCSI_RESET resets.
#define SG_SCSI_RESET_NOTHING 0
#define SG_SCSI_RESET_DEVICE 1
#define SG_SCSI_RESET_BUS 2
#define SG_SCSI_RESET_HOST 3

// The driver's defaults and limits.
#define SG_SCATTER_SZ (8 * 4096)
#define SG_DEFAULT_RETRIES 0
#define SG_DEF_FORCE_LOW_DMA 0
#define SG_DEF_FORCE_PACK_ID 0
#define SG_DEF_KEEP_ORPHAN 0
#define SG_DEF_RESERVED_SIZE SG_SCATTER_SZ
#define SG_MAX_QUEUE 16
#define SG_BIG_BUFF SG_DEF_RESERVED_SIZE

// The length of the sense data an sg_header carries.
#define SG_MAX_SENSE 16

// Requests kept from the version 2 driver.
#define SG_SET_TIMEOUT 0x2201
#define SG_GET_TIMEOUT 0x2202
#define SG_GET_COMMAND_Q 0x2270
#define SG_SET_COMMAND_Q 0x2271
#define SG_SET_DEBUG 0x227e
#define SG_NEXT_CMD_LEN 0x2283

// A minute, in the ticks SG_SET_TIMEOUT counts, of which there are a hundred
// to the second.
#define SG_DEFAULT_TIMEOUT (60 * 100)
#define SG_DEF_COMMAND_Q 0
#define SG_DEF_UNDERRUN_FLAG 0

#endif // LLVM_LIBC_MACROS_SCSI_SG_MACROS_H
