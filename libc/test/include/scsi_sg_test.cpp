//===-- Unittests for scsi/sg.h -------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "include/llvm-libc-macros/scsi-sg-macros.h"
#include "include/llvm-libc-types/struct_ccs_modesel_head.h"
#include "include/llvm-libc-types/struct_sg_header.h"
#include "include/llvm-libc-types/struct_sg_io_hdr.h"
#include "include/llvm-libc-types/struct_sg_iovec.h"
#include "include/llvm-libc-types/struct_sg_req_info.h"
#include "include/llvm-libc-types/struct_sg_scsi_id.h"
#include "test/UnitTest/Test.h"

// The driver copies these to and from the caller whole, so each has to be the
// size the kernel's own definition is.
TEST(LlvmLibcScsiSgTest, StructuresAreTheSizesTheDriverCopies) {
  bool lp64 = sizeof(void *) == 8;
  EXPECT_EQ(sizeof(sg_io_hdr_t), size_t(lp64 ? 88 : 64));
  EXPECT_EQ(sizeof(sg_iovec_t), 2 * sizeof(void *));
  EXPECT_EQ(sizeof(struct sg_scsi_id), size_t(32));
  EXPECT_EQ(sizeof(sg_req_info_t), size_t(lp64 ? 24 : 20));
  EXPECT_EQ(sizeof(struct sg_header), size_t(36));
  EXPECT_EQ(sizeof(struct ccs_modesel_head), size_t(12));
}

// The five bit fields of the old header share one unsigned int between them,
// with nothing left over.
TEST(LlvmLibcScsiSgTest, OldHeaderFlagsFillOneWord) {
  struct sg_header header;
  EXPECT_EQ(sizeof(header.sense_buffer), size_t(SG_MAX_SENSE));
  EXPECT_EQ(sizeof(header) - sizeof(header.sense_buffer),
            5 * sizeof(unsigned int));
}

TEST(LlvmLibcScsiSgTest, OlderNamesAgree) {
  Sg_io_hdr *io = static_cast<sg_io_hdr_t *>(nullptr);
  Sg_req_info *req = static_cast<sg_req_info_t *>(nullptr);
  Sg_scsi_id *id = static_cast<struct sg_scsi_id *>(nullptr);
  EXPECT_TRUE(io == nullptr && req == nullptr && id == nullptr);
  EXPECT_EQ(SG_BIG_BUFF, SG_SCATTER_SZ);
}
