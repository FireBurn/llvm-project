//===-- Unittests for the gshadow line parser -----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/types/struct_sgrp.h"
#include "src/__support/CPP/span.h"
#include "src/gshadow/gshadow_utils.h"
#include "src/gshadow/sgetsgent.h"
#include "src/gshadow/sgetsgent_r.h"
#include "test/UnitTest/Test.h"

using LIBC_NAMESPACE::cpp::span;

namespace {

bool parse(char *line, struct sgrp *entry) {
  size_t len = 0;
  while (line[len] != '\0')
    ++len;
  return LIBC_NAMESPACE::internal::parse_line(span<char>(line, len + 1), entry);
}

} // anonymous namespace

TEST(LlvmLibcGShadowTest, BothListsSet) {
  char line[] = "wheel:$6$hash:alice:alice,bob,carol";
  struct sgrp entry;
  ASSERT_TRUE(parse(line, &entry));
  ASSERT_STREQ(entry.sg_namp, "wheel");
  ASSERT_STREQ(entry.sg_passwd, "$6$hash");
  ASSERT_STREQ(entry.sg_adm[0], "alice");
  ASSERT_TRUE(entry.sg_adm[1] == nullptr);
  ASSERT_STREQ(entry.sg_mem[0], "alice");
  ASSERT_STREQ(entry.sg_mem[1], "bob");
  ASSERT_STREQ(entry.sg_mem[2], "carol");
  ASSERT_TRUE(entry.sg_mem[3] == nullptr);
}

TEST(LlvmLibcGShadowTest, EmptyListsAreEmptyNotAbsent) {
  char line[] = "root:*::";
  struct sgrp entry;
  ASSERT_TRUE(parse(line, &entry));
  ASSERT_STREQ(entry.sg_namp, "root");
  ASSERT_STREQ(entry.sg_passwd, "*");
  ASSERT_TRUE(entry.sg_adm != nullptr);
  ASSERT_TRUE(entry.sg_adm[0] == nullptr);
  ASSERT_TRUE(entry.sg_mem != nullptr);
  ASSERT_TRUE(entry.sg_mem[0] == nullptr);
}

TEST(LlvmLibcGShadowTest, MembersWithoutAdmins) {
  char line[] = "adm:!::alice,bob";
  struct sgrp entry;
  ASSERT_TRUE(parse(line, &entry));
  ASSERT_TRUE(entry.sg_adm[0] == nullptr);
  ASSERT_STREQ(entry.sg_mem[0], "alice");
  ASSERT_STREQ(entry.sg_mem[1], "bob");
  ASSERT_TRUE(entry.sg_mem[2] == nullptr);
}

TEST(LlvmLibcGShadowTest, MissingListsAreEmptyOnes) {
  // A line which stops after the password is still an entry; the two lists
  // are simply empty. This is what glibc reads such a line as.
  char line[] = "group:x";
  struct sgrp entry;
  ASSERT_TRUE(parse(line, &entry));
  ASSERT_STREQ(entry.sg_namp, "group");
  ASSERT_TRUE(entry.sg_adm[0] == nullptr);
  ASSERT_TRUE(entry.sg_mem[0] == nullptr);

  char one_list[] = "h:x:a";
  ASSERT_TRUE(parse(one_list, &entry));
  ASSERT_STREQ(entry.sg_adm[0], "a");
  ASSERT_TRUE(entry.sg_mem[0] == nullptr);
}

TEST(LlvmLibcGShadowTest, MemberListRunsToTheEndOfTheLine) {
  // Nothing follows the member list, so a colon in it is part of a name
  // rather than the start of another field.
  char line[] = "g:x:a:b:extra";
  struct sgrp entry;
  ASSERT_TRUE(parse(line, &entry));
  ASSERT_STREQ(entry.sg_adm[0], "a");
  ASSERT_STREQ(entry.sg_mem[0], "b:extra");
  ASSERT_TRUE(entry.sg_mem[1] == nullptr);
}

TEST(LlvmLibcGShadowTest, EmptyNameIsAllowed) {
  char line[] = ":x::";
  struct sgrp entry;
  ASSERT_TRUE(parse(line, &entry));
  ASSERT_STREQ(entry.sg_namp, "");
}

TEST(LlvmLibcGShadowTest, SplitNamesRespectsTheRoomGiven) {
  char field[] = "a,b,c";
  char *out[4];
  ASSERT_TRUE(LIBC_NAMESPACE::gshadow::split_names(
      span<char>(field, sizeof(field)), out, 4));
  ASSERT_STREQ(out[0], "a");
  ASSERT_TRUE(out[3] == nullptr);

  // Three names and the terminating null do not fit in three slots.
  char again[] = "a,b,c";
  char *small[3];
  ASSERT_FALSE(LIBC_NAMESPACE::gshadow::split_names(
      span<char>(again, sizeof(again)), small, 3));
}

TEST(LlvmLibcGShadowTest, ALineMayStopAfterTheName) {
  // What glibc makes of a line with none of the separators: the fields it
  // leaves out are empty ones.
  char line[] = "nofields";
  struct sgrp entry;
  ASSERT_TRUE(parse(line, &entry));
  ASSERT_STREQ(entry.sg_namp, "nofields");
  ASSERT_STREQ(entry.sg_passwd, "");
  ASSERT_TRUE(entry.sg_adm[0] == nullptr);
  ASSERT_TRUE(entry.sg_mem[0] == nullptr);
}

TEST(LlvmLibcGShadowTest, SgetsgentReadsALineOfItsOwn) {
  struct sgrp *entry = LIBC_NAMESPACE::sgetsgent("wheel:!:root:alice,bob");
  ASSERT_TRUE(entry != nullptr);
  ASSERT_STREQ(entry->sg_namp, "wheel");
  ASSERT_STREQ(entry->sg_passwd, "!");
  ASSERT_STREQ(entry->sg_adm[0], "root");
  ASSERT_TRUE(entry->sg_adm[1] == nullptr);
  ASSERT_STREQ(entry->sg_mem[0], "alice");
  ASSERT_STREQ(entry->sg_mem[1], "bob");
  ASSERT_TRUE(entry->sg_mem[2] == nullptr);
}

TEST(LlvmLibcGShadowTest, SgetsgentRAnswersInTheCallersBuffer) {
  struct sgrp entry;
  char buffer[1024];
  struct sgrp *result = nullptr;
  ASSERT_EQ(LIBC_NAMESPACE::sgetsgent_r("audio:*:admin:m1,m2", &entry, buffer,
                                        sizeof(buffer), &result),
            0);
  ASSERT_TRUE(result == &entry);
  ASSERT_STREQ(entry.sg_namp, "audio");
  ASSERT_STREQ(entry.sg_adm[0], "admin");
  ASSERT_STREQ(entry.sg_mem[0], "m1");
  ASSERT_STREQ(entry.sg_mem[1], "m2");
  ASSERT_TRUE(entry.sg_mem[2] == nullptr);
  // The line was copied, so nothing points back at what was passed in.
  ASSERT_TRUE(entry.sg_namp >= buffer &&
              entry.sg_namp < buffer + sizeof(buffer));
}

TEST(LlvmLibcGShadowTest, SgetsgentRSaysWhenTheBufferIsTooSmall) {
  struct sgrp entry;
  char buffer[8];
  struct sgrp *result = &entry;
  ASSERT_NE(LIBC_NAMESPACE::sgetsgent_r("audio:*:admin:m1,m2", &entry, buffer,
                                        sizeof(buffer), &result),
            0);
  ASSERT_TRUE(result == nullptr);
}

TEST(LlvmLibcGShadowTest, TheReentrantFormRejectsWhatItCannotAnswerInto) {
  struct sgrp entry;
  char buffer[1024];
  struct sgrp *result = nullptr;
  ASSERT_NE(LIBC_NAMESPACE::sgetsgent_r(nullptr, &entry, buffer, sizeof(buffer),
                                        &result),
            0);
  ASSERT_NE(LIBC_NAMESPACE::sgetsgent_r("a:b:c:d", nullptr, buffer,
                                        sizeof(buffer), &result),
            0);
  ASSERT_NE(LIBC_NAMESPACE::sgetsgent_r("a:b:c:d", &entry, nullptr,
                                        sizeof(buffer), &result),
            0);
  ASSERT_NE(LIBC_NAMESPACE::sgetsgent_r("a:b:c:d", &entry, buffer,
                                        sizeof(buffer), nullptr),
            0);
}
