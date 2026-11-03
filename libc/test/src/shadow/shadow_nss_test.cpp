//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for shadow lookups through the name service switch.
///
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/types/struct_spwd.h"
#include "src/__support/CPP/string_view.h"
#include "src/__support/nss/config.h"
#include "src/__support/nss/module.h"
#include "src/shadow/endspent.h"
#include "src/shadow/getspent.h"
#include "src/shadow/getspnam.h"
#include "src/shadow/getspnam_r.h"
#include "src/shadow/setspent.h"
#include "src/shadow/shadow_utils.h"
#include "src/stdio/fclose.h"
#include "src/stdio/fopen.h"
#include "src/stdio/fwrite.h"
#include "src/stdio/remove.h"
#include "src/string/memcpy.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/Test.h"

using LIBC_NAMESPACE::cpp::string_view;
using LIBC_NAMESPACE::nss::Status;

namespace {

constexpr char FILE_CONTENTS[] = "alice:$y$alice:19500:1:180:14:30:20000:0\n"
                                 "bob:!:19600:0:99999:7:::\n";

// A module named "fake" which knows one user and two entries to enumerate.
int spent_position = 0;
int setspent_calls = 0;
int endspent_calls = 0;

Status fake_getspnam_r(const char *name, struct spwd *sp, char *buf,
                       size_t len, int *errnop) {
  if (string_view(name) != "ghost")
    return LIBC_NAMESPACE::nss::NOTFOUND;
  if (len < 16) {
    *errnop = ERANGE;
    return LIBC_NAMESPACE::nss::TRYAGAIN;
  }
  LIBC_NAMESPACE::memcpy(buf, "ghost\0!\0", 8);
  sp->sp_namp = buf;
  sp->sp_pwdp = buf + 6;
  sp->sp_lstchg = 12345;
  return LIBC_NAMESPACE::nss::SUCCESS;
}

Status fake_setspent(int) {
  ++setspent_calls;
  spent_position = 0;
  return LIBC_NAMESPACE::nss::SUCCESS;
}

Status fake_endspent(void) {
  ++endspent_calls;
  return LIBC_NAMESPACE::nss::SUCCESS;
}

Status fake_getspent_r(struct spwd *sp, char *buf, size_t len, int *errnop) {
  static const char names[2][8] = {"first", "second"};
  if (spent_position >= 2)
    return LIBC_NAMESPACE::nss::NOTFOUND;
  if (len < 16) {
    *errnop = ERANGE;
    return LIBC_NAMESPACE::nss::TRYAGAIN;
  }
  LIBC_NAMESPACE::memcpy(buf, names[spent_position++], 8);
  sp->sp_namp = buf;
  sp->sp_pwdp = buf + 7;
  buf[7] = '\0';
  return LIBC_NAMESPACE::nss::SUCCESS;
}

void *resolver(const char *service, const char *function) {
  if (string_view(service) != "fake")
    return nullptr;
  string_view f(function);
  if (f == "getspnam_r")
    return reinterpret_cast<void *>(&fake_getspnam_r);
  if (f == "setspent")
    return reinterpret_cast<void *>(&fake_setspent);
  if (f == "endspent")
    return reinterpret_cast<void *>(&fake_endspent);
  if (f == "getspent_r")
    return reinterpret_cast<void *>(&fake_getspent_r);
  return nullptr;
}

class LlvmLibcShadowNssTest : public LIBC_NAMESPACE::testing::ErrnoCheckingTest {
public:
  void SetUp() override {
    LIBC_NAMESPACE::testing::ErrnoCheckingTest::SetUp();
    path = libc_make_test_file_path("shadow_nss_test.shadow");
    ::FILE *f = LIBC_NAMESPACE::fopen(path, "w");
    ASSERT_FALSE(f == nullptr);
    ASSERT_EQ(
        LIBC_NAMESPACE::fwrite(FILE_CONTENTS, 1, sizeof(FILE_CONTENTS) - 1, f),
        sizeof(FILE_CONTENTS) - 1);
    ASSERT_EQ(LIBC_NAMESPACE::fclose(f), 0);
    LIBC_NAMESPACE::shadow_db::TESTONLY_set_shadow_path(path);
    LIBC_NAMESPACE::nss::TESTONLY_set_config_path("/nonexistent/nsswitch.conf");
    LIBC_NAMESPACE::nss::TESTONLY_set_resolver(resolver);
    ASSERT_TRUE(LIBC_NAMESPACE::nss::configure("shadow", "files fake"));
    spent_position = setspent_calls = endspent_calls = 0;
  }

  void TearDown() override {
    LIBC_NAMESPACE::endspent();
    LIBC_NAMESPACE::nss::TESTONLY_set_resolver(nullptr);
    LIBC_NAMESPACE::nss::TESTONLY_reset_config_path();
    LIBC_NAMESPACE::remove(path);
    LIBC_NAMESPACE::testing::ErrnoCheckingTest::TearDown();
  }

  const char *path;
};

} // anonymous namespace

TEST_F(LlvmLibcShadowNssTest, FileFirstThenModule) {
  struct spwd *alice = LIBC_NAMESPACE::getspnam("alice");
  ASSERT_TRUE(alice != nullptr);
  ASSERT_EQ(alice->sp_lstchg, 19500L);
  struct spwd *ghost = LIBC_NAMESPACE::getspnam("ghost");
  ASSERT_TRUE(ghost != nullptr);
  ASSERT_EQ(ghost->sp_lstchg, 12345L);
  ASSERT_TRUE(LIBC_NAMESPACE::getspnam("nobody-at-all") == nullptr);
}

TEST_F(LlvmLibcShadowNssTest, ReentrantLookupReportsASmallBuffer) {
  struct spwd sp;
  struct spwd *result;
  char small[8];
  ASSERT_EQ(
      LIBC_NAMESPACE::getspnam_r("ghost", &sp, small, sizeof(small), &result),
      ERANGE);
  char big[256];
  ASSERT_EQ(LIBC_NAMESPACE::getspnam_r("ghost", &sp, big, sizeof(big), &result),
            0);
  ASSERT_TRUE(result == &sp);
  ASSERT_EQ(sp.sp_lstchg, 12345L);
  ASSERT_EQ(LIBC_NAMESPACE::getspnam_r("nobody-at-all", &sp, big, sizeof(big),
                                       &result),
            0);
  ASSERT_TRUE(result == nullptr);
}

TEST_F(LlvmLibcShadowNssTest, ALookupLeavesAnEnumerationAlone) {
  LIBC_NAMESPACE::setspent();
  struct spwd *first = LIBC_NAMESPACE::getspent();
  ASSERT_TRUE(first != nullptr);
  ASSERT_EQ(first->sp_lstchg, 19500L);
  ASSERT_TRUE(LIBC_NAMESPACE::getspnam("bob") != nullptr);
  struct spwd *next = LIBC_NAMESPACE::getspent();
  ASSERT_TRUE(next != nullptr);
  ASSERT_EQ(next->sp_lstchg, 19600L);
}

TEST_F(LlvmLibcShadowNssTest, EnumerationVisitsEverySource) {
  LIBC_NAMESPACE::setspent();
  char seen[8];
  int n = 0;
  while (struct spwd *sp = LIBC_NAMESPACE::getspent()) {
    ASSERT_TRUE(n < 8);
    seen[n++] = sp->sp_namp[0];
  }
  ASSERT_EQ(n, 4);
  ASSERT_TRUE(seen[0] == 'a' && seen[1] == 'b');
  ASSERT_TRUE(seen[2] == 'f' && seen[3] == 's');
  ASSERT_EQ(setspent_calls, 1);
  ASSERT_EQ(endspent_calls, 1);
  LIBC_NAMESPACE::endspent();
}
