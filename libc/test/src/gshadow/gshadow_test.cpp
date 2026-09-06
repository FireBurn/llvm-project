//===-- Unittests for the shadow group database ---------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/types/FILE.h"
#include "hdr/types/struct_sgrp.h"
#include "src/gshadow/endsgent.h"
#include "src/gshadow/fgetsgent.h"
#include "src/gshadow/fgetsgent_r.h"
#include "src/gshadow/getsgent.h"
#include "src/gshadow/getsgent_r.h"
#include "src/gshadow/getsgnam.h"
#include "src/gshadow/getsgnam_r.h"
#include "src/gshadow/gshadow_utils.h"
#include "src/gshadow/putsgent.h"
#include "src/gshadow/setsgent.h"
#include "src/stdio/fclose.h"
#include "src/stdio/fopen.h"
#include "src/stdio/fwrite.h"
#include "src/stdio/remove.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/Test.h"

namespace {

constexpr char FILE_CONTENTS[] = "wheel:$y$hash:root:alice,bob\n"
                                 "empty:!::\n";

class LlvmLibcGshadowTest : public LIBC_NAMESPACE::testing::ErrnoCheckingTest {
public:
  void SetUp() override {
    LIBC_NAMESPACE::testing::ErrnoCheckingTest::SetUp();
    path = libc_make_test_file_path("gshadow_test.gshadow");
    ::FILE *f = LIBC_NAMESPACE::fopen(path, "w");
    ASSERT_FALSE(f == nullptr);
    ASSERT_EQ(
        LIBC_NAMESPACE::fwrite(FILE_CONTENTS, 1, sizeof(FILE_CONTENTS) - 1, f),
        sizeof(FILE_CONTENTS) - 1);
    ASSERT_EQ(LIBC_NAMESPACE::fclose(f), 0);
    LIBC_NAMESPACE::gshadow_db::TESTONLY_set_gshadow_path(path);
  }

  void TearDown() override {
    LIBC_NAMESPACE::endsgent();
    LIBC_NAMESPACE::remove(path);
    LIBC_NAMESPACE::testing::ErrnoCheckingTest::TearDown();
  }

  const char *path;
};

} // anonymous namespace

TEST_F(LlvmLibcGshadowTest, LookUpByName) {
  struct sgrp *entry = LIBC_NAMESPACE::getsgnam("wheel");
  ASSERT_FALSE(entry == nullptr);
  ASSERT_STREQ(entry->sg_namp, "wheel");
  ASSERT_STREQ(entry->sg_passwd, "$y$hash");
  ASSERT_FALSE(entry->sg_adm == nullptr);
  ASSERT_STREQ(entry->sg_adm[0], "root");
  ASSERT_TRUE(entry->sg_adm[1] == nullptr);
  ASSERT_FALSE(entry->sg_mem == nullptr);
  ASSERT_STREQ(entry->sg_mem[0], "alice");
  ASSERT_STREQ(entry->sg_mem[1], "bob");
  ASSERT_TRUE(entry->sg_mem[2] == nullptr);
}

TEST_F(LlvmLibcGshadowTest, GroupWithNobodyInIt) {
  struct sgrp *entry = LIBC_NAMESPACE::getsgnam("empty");
  ASSERT_FALSE(entry == nullptr);
  ASSERT_STREQ(entry->sg_namp, "empty");
  ASSERT_FALSE(entry->sg_adm == nullptr);
  ASSERT_TRUE(entry->sg_adm[0] == nullptr);
  ASSERT_FALSE(entry->sg_mem == nullptr);
  ASSERT_TRUE(entry->sg_mem[0] == nullptr);
}

TEST_F(LlvmLibcGshadowTest, NameThatIsNotThere) {
  ASSERT_TRUE(LIBC_NAMESPACE::getsgnam("nogroup") == nullptr);
}

TEST_F(LlvmLibcGshadowTest, WalkTheWholeFile) {
  LIBC_NAMESPACE::setsgent();

  struct sgrp *first = LIBC_NAMESPACE::getsgent();
  ASSERT_FALSE(first == nullptr);
  ASSERT_STREQ(first->sg_namp, "wheel");

  struct sgrp *second = LIBC_NAMESPACE::getsgent();
  ASSERT_FALSE(second == nullptr);
  ASSERT_STREQ(second->sg_namp, "empty");

  ASSERT_TRUE(LIBC_NAMESPACE::getsgent() == nullptr);

  LIBC_NAMESPACE::setsgent();
  struct sgrp *again = LIBC_NAMESPACE::getsgent();
  ASSERT_FALSE(again == nullptr);
  ASSERT_STREQ(again->sg_namp, "wheel");
}

TEST_F(LlvmLibcGshadowTest, ReadFromAStream) {
  ::FILE *f = LIBC_NAMESPACE::fopen(path, "r");
  ASSERT_FALSE(f == nullptr);

  struct sgrp *first = LIBC_NAMESPACE::fgetsgent(f);
  ASSERT_FALSE(first == nullptr);
  ASSERT_STREQ(first->sg_namp, "wheel");

  struct sgrp *second = LIBC_NAMESPACE::fgetsgent(f);
  ASSERT_FALSE(second == nullptr);
  ASSERT_STREQ(second->sg_namp, "empty");

  ASSERT_TRUE(LIBC_NAMESPACE::fgetsgent(f) == nullptr);
  ASSERT_EQ(LIBC_NAMESPACE::fclose(f), 0);
}

TEST_F(LlvmLibcGshadowTest, CallerSuppliedBuffers) {
  struct sgrp entry;
  char buf[256];
  struct sgrp *result = nullptr;

  ASSERT_EQ(
      LIBC_NAMESPACE::getsgnam_r("wheel", &entry, buf, sizeof(buf), &result),
      0);
  ASSERT_TRUE(result == &entry);
  ASSERT_STREQ(entry.sg_namp, "wheel");
  ASSERT_STREQ(entry.sg_mem[0], "alice");

  ASSERT_EQ(
      LIBC_NAMESPACE::getsgnam_r("nogroup", &entry, buf, sizeof(buf), &result),
      0);
  ASSERT_TRUE(result == nullptr);

  LIBC_NAMESPACE::setsgent();
  ASSERT_EQ(LIBC_NAMESPACE::getsgent_r(&entry, buf, sizeof(buf), &result), 0);
  ASSERT_TRUE(result == &entry);
  ASSERT_STREQ(entry.sg_namp, "wheel");

  ::FILE *f = LIBC_NAMESPACE::fopen(path, "r");
  ASSERT_FALSE(f == nullptr);
  ASSERT_EQ(LIBC_NAMESPACE::fgetsgent_r(f, &entry, buf, sizeof(buf), &result),
            0);
  ASSERT_TRUE(result == &entry);
  ASSERT_STREQ(entry.sg_namp, "wheel");
  ASSERT_EQ(LIBC_NAMESPACE::fclose(f), 0);
}

TEST_F(LlvmLibcGshadowTest, BufferTooSmall) {
  struct sgrp entry;
  char buf[4];
  struct sgrp *result = nullptr;
  ASSERT_EQ(
      LIBC_NAMESPACE::getsgnam_r("wheel", &entry, buf, sizeof(buf), &result),
      ERANGE);
  ASSERT_TRUE(result == nullptr);
}

TEST_F(LlvmLibcGshadowTest, WriteAnEntryOut) {
  auto out = libc_make_test_file_path("gshadow_test.out");
  ::FILE *f = LIBC_NAMESPACE::fopen(out, "w");
  ASSERT_FALSE(f == nullptr);

  struct sgrp *entry = LIBC_NAMESPACE::getsgnam("wheel");
  ASSERT_FALSE(entry == nullptr);
  ASSERT_EQ(LIBC_NAMESPACE::putsgent(entry, f), 0);
  ASSERT_EQ(LIBC_NAMESPACE::fclose(f), 0);

  ::FILE *r = LIBC_NAMESPACE::fopen(out, "r");
  ASSERT_FALSE(r == nullptr);
  struct sgrp *read = LIBC_NAMESPACE::fgetsgent(r);
  ASSERT_FALSE(read == nullptr);
  ASSERT_STREQ(read->sg_namp, "wheel");
  ASSERT_STREQ(read->sg_mem[0], "alice");
  ASSERT_STREQ(read->sg_mem[1], "bob");
  ASSERT_EQ(LIBC_NAMESPACE::fclose(r), 0);
  ASSERT_EQ(LIBC_NAMESPACE::remove(out), 0);
}
